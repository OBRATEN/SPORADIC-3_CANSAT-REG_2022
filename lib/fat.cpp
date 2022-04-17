#include "fat.hpp"

uint32_t SD_volume::_cacheBlockNumber = 0xFFFFFFFF;
cache_t  SD_volume::_cacheBuffer;
SD_card* SD_volume::_sdCard;
uint8_t  SD_volume::_cacheDirty = 0;
uint32_t SD_volume::_cacheMirrorBlock = 0;

uint8_t SD_volume::allocContiguous(uint32_t count, uint32_t* curCluster) {
  uint32_t bgnCluster;
  uint8_t setStart;
  if (*curCluster) {
    bgnCluster = *curCluster + 1;
    setStart = false;
  } else {
    bgnCluster = _allocSearchStart;
    setStart = 1 == count;
  }
  uint32_t endCluster = bgnCluster;
  uint32_t fatEnd = _clusterCount + 1;
  for (uint32_t n = 0;; n++, endCluster++) {
    if (n >= _clusterCount) return false;
    if (endCluster > fatEnd) bgnCluster = endCluster = 2;
    uint32_t f;
    if (!fatGet(endCluster, &f)) return false;
    if (f != 0) bgnCluster = endCluster + 1;
    else if ((endCluster - bgnCluster + 1) == count) break;
  }
  if (!fatPutEOC(endCluster)) return false;
  while (endCluster > bgnCluster) {
    if (!fatPut(endCluster - 1, endCluster)) return false;
    endCluster--;
  }
  if (*curCluster != 0) if (!fatPut(*curCluster, bgnCluster)) return false;
  *curCluster = bgnCluster;
  if (setStart) _allocSearchStart = bgnCluster + 1;
  return true;
}

uint8_t SD_volume::cacheFlush(void) {
  if (_cacheDirty) {
    if (!_sdCard->writeBlock(_cacheBlockNumber, _cacheBuffer.data)) return false;
    if (_cacheMirrorBlock) {
      if (!_sdCard->writeBlock(_cacheMirrorBlock, _cacheBuffer.data)) return false;
      _cacheMirrorBlock = 0;
    } _cacheDirty = 0;
  } return true;
}

uint8_t SD_volume::cacheRawBlock(uint32_t blockNumber, uint8_t action) {
  if (_cacheBlockNumber != blockNumber) {
    if (!cacheFlush()) return false;
    if (!_sdCard->readBlock(blockNumber, _cacheBuffer.data)) return false;
    _cacheBlockNumber = blockNumber;
  }
  _cacheDirty |= action;
  return true;
}

uint8_t SD_volume::cacheZeroBlock(uint32_t blockNumber) {
  if (!cacheFlush()) return false;
  for (uint16_t i = 0; i < 512; i++) _cacheBuffer.data[i] = 0;
  _cacheBlockNumber = blockNumber;
  cacheSetDirty();
  return true;
}

uint8_t SD_volume::chainSize(uint32_t cluster, uint32_t* size) const {
  uint32_t s = 0;
  do {
    if (!fatGet(cluster, &cluster)) return false;
    s += 512UL << _clusterSizeShift;
  } while (!isEOC(cluster));
  *size = s;
  return true;
}

uint8_t SD_volume::fatGet(uint32_t cluster, uint32_t* value) const {
  if (cluster > (_clusterCount + 1)) return false;
  uint32_t lba = _fatStartBlock;
  lba += _fatType == 16 ? cluster >> 8 : cluster >> 7;
  if (lba != _cacheBlockNumber) if (!cacheRawBlock(lba, CACHE_FOR_READ)) return false;
  if (_fatType == 16) *value = _cacheBuffer.fat16[cluster & 0xFF];
  else *value = _cacheBuffer.fat32[cluster & 0x7F] & FAT32MASK;
  return true;
}

uint8_t SD_volume::fatPut(uint32_t cluster, uint32_t value) {
  if (cluster < 2) return false;
  if (cluster > (_clusterCount + 1)) return false;
  uint32_t lba = _fatStartBlock;
  lba += _fatType == 16 ? cluster >> 8 : cluster >> 7;
  if (lba != _cacheBlockNumber) if (!cacheRawBlock(lba, CACHE_FOR_READ)) return false;
  if (_fatType == 16) _cacheBuffer.fat16[cluster & 0XFF] = value;
  else _cacheBuffer.fat32[cluster & 0X7F] = value;
  cacheSetDirty();
  if (_fatCount > 1) _cacheMirrorBlock = lba + _blocksPerFat;
  return true;
}

uint8_t SD_volume::freeChain(uint32_t cluster) {
  _allocSearchStart = 2;
  do {
    uint32_t next;
    if (!fatGet(cluster, &next)) return false;
    if (!fatPut(cluster, 0)) return false;
    cluster = next;
  } while (!isEOC(cluster));
  return true;
}

uint8_t SD_volume::init(SD_card* dev, uint8_t part) {
  uint32_t volumeStartBlock = 0;
  _sdCard = dev;
  if (part) {
    if (part > 4) return false;
    if (!cacheRawBlock(volumeStartBlock, CACHE_FOR_READ)) return false;
    part_t* p = &_cacheBuffer.mbr.part[part-1];
    if ((p->boot & 0x7F) !=0  ||
      p->totalSectors < 100 ||
      p->firstSector == 0) {
      return false;
    }
    volumeStartBlock = p->firstSector;
  }
  if (!cacheRawBlock(volumeStartBlock, CACHE_FOR_READ)) return false;
  bpb_t* bpb = &_cacheBuffer.fbs.bpb;
  if (bpb->bytesPerSector != 512    ||
      bpb->fatCount == 0            ||
      bpb->reservedSectorCount == 0 ||
      bpb->sectorsPerCluster == 0) {
      return false;
  }
  _fatCount = bpb->fatCount;
  _blocksPerCluster = bpb->sectorsPerCluster;
  _clusterSizeShift = 0;
  while (_blocksPerCluster != (1 << _clusterSizeShift))
    if (_clusterSizeShift++ > 7) return false;
  _blocksPerFat = bpb->sectorsPerFat16 ?
                  bpb->sectorsPerFat16 : bpb->sectorsPerFat32;
  _fatStartBlock = volumeStartBlock + bpb->reservedSectorCount;
  _rootDirEntryCount = bpb->rootDirEntryCount;
  _rootDirStart = _fatStartBlock + bpb->fatCount * _blocksPerFat;
  _dataStartBlock = _rootDirStart + ((32 * bpb->rootDirEntryCount + 511)/512);
  uint32_t totalBlocks = bpb->totalSectors16 ?
                         bpb->totalSectors16 : bpb->totalSectors32;
  _clusterCount = totalBlocks - (_dataStartBlock - volumeStartBlock);
  _clusterCount >>= _clusterSizeShift;
  if (_clusterCount < 4085) _fatType = 12;
  else if (_clusterCount < 65525) _fatType = 16;
  else {
    _rootDirStart = bpb->fat32RootCluster;
    _fatType = 32;
  } return true;
}


