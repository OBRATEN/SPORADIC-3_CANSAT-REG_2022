#ifndef FAT_H
#define FAT_H

/* Библиотека для работы с фс FAT
 * Автор: Гарагуля Артур, "SPORADIC", г. Курск
 * Зависимости: sd_util, avr, spi
 */

#include <stddef.h>
#include "sd_util.hpp"

#define ALLOW_DEPRECATED_FUNCTIONS 1

// Байт 510 блока BOOT или MBR
#define BOOTSIG0 0x55
// Байт 511 блока BOOT или MBR
#define BOOTSIG1 0xAA

#define LS_DATE  1
#define LS_SIZE  2
#define LS_R     4

#define O_READ    0x01
#define O_RDONLY  O_READ
#define O_WRITE   0x02
#define O_WRONLY  O_WRITE
#define O_RDWR    (O_READ | O_WRITE)
#define O_ACCMODE (O_READ | O_WRITE)
#define O_APPEND  0x04
#define O_SYNC    0x08
#define O_CREAT   0x10
#define O_EXCL    0x20
#define O_TRUNC   0x40

#define T_ACCESS 1
#define T_CREATE 2
#define T_WRITE  4

#define FAT_FILE_TYPE_CLOSED  0
#define FAT_FILE_TYPE_NORMAL  1
#define FAT_FILE_TYPE_ROOT16  2
#define FAT_FILE_TYPE_ROOT32  3
#define FAT_FILE_TYPE_SUBDIR  4
#define FAT_FILE_TYPE_MIN_DIR FAT_FILE_TYPE_ROOT16

#define NOINLINE __attribute__((noinline,unused))
#define UNUSEDOK __attribute__((unused))

class SD_volume;

static inline uint16_t FAT_DATE(uint16_t year, uint8_t month, uint8_t day) {
  return (year - 1980) << 9 | month << 5 | day;
}

static inline uint16_t FAT_YEAR(uint16_t fatDate) {
  return 1980 + (fatDate >> 9);
}

static inline uint8_t FAT_MONTH(uint16_t fatDate) {
  return (fatDate >> 5) & 0XF;
}

static inline uint8_t FAT_DAY(uint16_t fatDate) {
  return fatDate & 0X1F;
}

static inline uint16_t FAT_TIME(uint8_t hour, uint8_t minute, uint8_t second) {
  return hour << 11 | minute << 5 | second >> 1;
}

static inline uint8_t FAT_HOUR(uint16_t fatTime) {
  return fatTime >> 11;
}

static inline uint8_t FAT_MINUTE(uint16_t fatTime) {
  return(fatTime >> 5) & 0X3F;
}

static inline uint8_t FAT_SECOND(uint16_t fatTime) {
  return 2*(fatTime & 0X1F);
}
uint16_t const FAT_DEFAULT_DATE = ((2000 - 1980) << 9) | (1 << 5) | 1;
uint16_t const FAT_DEFAULT_TIME = (1 << 11);

/* Структура таблицы разделов */
struct partionTable {
  uint8_t boot;
  uint8_t beginHead;
  uint16_t beginSector : 6;
  uint16_t beginCylinderHigh : 2;
  uint8_t  beginCylinderLow;
  uint8_t  type;
  uint8_t  endHead;
  uint16_t endSector : 6;
  uint16_t endCylinderHigh : 2;
  uint8_t  endCylinderLow;
  uint32_t firstSector;
  uint32_t totalSectors;
} __attribute__((packed));

typedef struct partionTable part_t;

struct masterBootRecord {
  uint8_t  codeArea[440];
  uint32_t diskSignature;
  uint16_t usuallyZero;
  part_t   part[4];
  uint8_t  mbrSig0;
  uint8_t  mbrSig1;
} __attribute__((packed));
typedef struct masterBootRecord mbr_t;

struct biosParmBlock {
  uint16_t bytesPerSector;
  uint8_t  sectorsPerCluster;
  uint16_t reservedSectorCount;
  uint8_t  fatCount;
  uint16_t rootDirEntryCount;
  uint16_t totalSectors16;
  uint8_t  mediaType;
  uint16_t sectorsPerFat16;
  uint16_t sectorsPerTrtack;
  uint16_t headCount;
  uint32_t hidddenSectors;
  uint32_t totalSectors32;
  uint32_t sectorsPerFat32;
  uint16_t fat32Flags;
  uint16_t fat32Version;
  uint32_t fat32RootCluster;
  uint16_t fat32FSInfo;
  uint16_t fat32BackBootBlock;
  uint8_t  fat32Reserved[12];
} __attribute__((packed));

typedef struct biosParmBlock bpb_t;

struct fat32BootSector {
  uint8_t  jmpToBootCode[3];
  char     oemName[8];
  bpb_t    bpb;
  uint8_t  driveNumber;
  uint8_t  reserved1;
  uint8_t  bootSignature;
  uint32_t volumeSerialNumber;
  char     volumeLabel[11];
  char     fileSystemType[8];
  uint8_t  bootCode[420];
  uint8_t  bootSectorSig0;
  uint8_t  bootSectorSig1;
} __attribute__((packed));
uint16_t const FAT16EOC = 0xFFFF;
uint16_t const FAT16EOC_MIN = 0xFFF8;
uint32_t const FAT32EOC = 0x0FFFFFFF;
uint32_t const FAT32EOC_MIN = 0x0FFFFFF8;
uint32_t const FAT32MASK = 0x0FFFFFFF;

typedef struct fat32BootSector fbs_t;

struct directoryEntry {
  uint8_t  name[11];
  uint8_t  attributes;
  uint8_t  reservedNT;
  uint8_t  creationTimeTenths;
  uint16_t creationTime;
  uint16_t creationDate;
  uint16_t lastAccessDate;
  uint16_t firstClusterHigh;
  uint16_t lastWriteTime;
  uint16_t lastWriteDate;
  uint16_t firstClusterLow;
  uint32_t fileSize;
} __attribute__((packed));

typedef struct directoryEntry dir_t;

uint8_t const DIR_NAME_0XE5 = 0X05;
uint8_t const DIR_NAME_DELETED = 0XE5;
uint8_t const DIR_NAME_FREE = 0X00;
uint8_t const DIR_ATT_READ_ONLY = 0X01;
uint8_t const DIR_ATT_HIDDEN = 0X02;
uint8_t const DIR_ATT_SYSTEM = 0X04;
uint8_t const DIR_ATT_VOLUME_ID = 0X08;
uint8_t const DIR_ATT_DIRECTORY = 0X10;
uint8_t const DIR_ATT_ARCHIVE = 0X20;
uint8_t const DIR_ATT_LONG_NAME = 0X0F;
uint8_t const DIR_ATT_LONG_NAME_MASK = 0X3F;
uint8_t const DIR_ATT_DEFINED_BITS = 0X3F;

static inline uint8_t DIR_IS_LONG_NAME(const dir_t* dir) {
  return (dir->attributes & DIR_ATT_LONG_NAME_MASK) == DIR_ATT_LONG_NAME;
}

uint8_t const DIR_ATT_FILE_TYPE_MASK = (DIR_ATT_VOLUME_ID | DIR_ATT_DIRECTORY);

static inline uint8_t DIR_IS_FILE(const dir_t* dir) {
  return (dir->attributes & DIR_ATT_FILE_TYPE_MASK) == 0;
}

static inline uint8_t DIR_IS_SUBDIR(const dir_t* dir) {
  return (dir->attributes & DIR_ATT_FILE_TYPE_MASK) == DIR_ATT_DIRECTORY;
}

static inline uint8_t DIR_IS_FILE_OR_SUBDIR(const dir_t* dir) {
  return (dir->attributes & DIR_ATT_VOLUME_ID) == 0;
}

static UNUSEDOK int FreeRam(void) {
  extern int  __bss_end;
  extern int* __brkval;
  int free_memory;
  if (reinterpret_cast<int>(__brkval) == 0) {
    free_memory = reinterpret_cast<int>(&free_memory)
                  - reinterpret_cast<int>(&__bss_end);
  } else {
    free_memory = reinterpret_cast<int>(&free_memory)
                  - reinterpret_cast<int>(__brkval);
  }
  return free_memory;
}

union cache_t {
  uint8_t  data[512];
  uint16_t fat16[256];
  uint32_t fat32[128];
  dir_t    dir[16];
  mbr_t    mbr;
  fbs_t    fbs;
};

class SD_file {
public:
  SD_file(void) : _type(FAT_FILE_TYPE_CLOSED) {}
  void clearUnbufferedRead(void) {_flags &= ~F_FILE_UNBUFFERED_READ;}
  uint8_t close(void);
  uint8_t contiguousRange(uint32_t* bgnBlock, uint32_t* endBlock);
  uint8_t createContiguous(SD_file* dirFile, const char* fileName, uint32_t size);
  uint32_t curCluster(void) const {return _curCluster;}
  uint32_t curPosition(void) const {return _curPosition;}
  static void dateTimeCallback(void (*dateTime)(uint16_t* date, uint16_t* time)) {
    _dateTime = dateTime;
  }
  static void dateTimeCallbackCancel(void) {_dateTime = 0;}
  uint32_t dirBlock(void) const {return _dirBlock;}
  uint8_t dirEntry(dir_t* dir);
  uint8_t dirIndex(void) const {return _dirIndex;}
  static void dirName(const dir_t& dir, char* name);
  uint32_t fileSize(void) const {return _fileSize;}
  uint32_t firstCluster(void) const {return _firstCluster;}
  uint8_t isDir(void) const {return _type >= FAT_FILE_TYPE_MIN_DIR;}
  uint8_t isFile(void) const {return _type == FAT_FILE_TYPE_NORMAL;}
  uint8_t isOpen(void) const {return _type != FAT_FILE_TYPE_CLOSED;}
  uint8_t isSubDir(void) const {return _type == FAT_FILE_TYPE_SUBDIR;}
  uint8_t isRoot(void) const {
    return _type == FAT_FILE_TYPE_ROOT16 || _type == FAT_FILE_TYPE_ROOT32;
  }
  void ls(uint8_t flags = 0, uint8_t indent = 0);
  uint8_t makeDir(SD_file* dir, const char* dirName);
  uint8_t open(SD_file* dirFile, uint16_t index, uint8_t oflag);
  uint8_t open(SD_file* dirFile, const char* fileName, uint8_t oflag);
  uint8_t openRoot(SD_volume* vol);
  static void printDirName(const dir_t& dir, uint8_t width);
  static void printFatDate(uint16_t fatDate);
  static void printFatTime(uint16_t fatTime);
  static void printTwoDigits(uint8_t v);
  int16_t read(void) {
    uint8_t b;
    return read(&b, 1) == 1 ? b : -1;
  }
  int16_t read(void* buf, uint16_t nbyte);
  int8_t readDir(dir_t* dir);
  static uint8_t remove(SD_file* dirFile, const char* fileName);
  uint8_t remove(void);
  void rewind(void) {_curPosition = _curCluster = 0;}
  uint8_t rmDir(void);
  uint8_t rmRfStar(void);
  uint8_t seekCur(uint32_t pos) {return seekSet(_curPosition + pos);}
  uint8_t seekEnd(void) {return seekSet(_fileSize);}
  uint8_t seekSet(uint32_t pos);
  void setUnbufferedRead(void) {if (isFile()) _flags |= F_FILE_UNBUFFERED_READ;}
  uint8_t timestamp(uint8_t flag, uint16_t year, uint8_t month, uint8_t day,
          uint8_t hour, uint8_t minute, uint8_t second);
  uint8_t sync(void);
  uint8_t type(void) const {return _type;}
  uint8_t truncate(uint32_t size);
  uint8_t unbufferedRead(void) const {return _flags & F_FILE_UNBUFFERED_READ;}
  SD_volume* volume(void) const {return _vol;}
  size_t write(uint8_t b);
  size_t write(const void* buf, uint16_t nbyte);
  size_t write(const char* str);
#if ALLOW_DEPRECATED_FUNCTIONS
  uint8_t contiguousRange(uint32_t& bgnBlock, uint32_t& endBlock) {
    return contiguousRange(&bgnBlock, &endBlock);
  }
  uint8_t createContiguous(SD_file& dirFile, const char* fileName, uint32_t size) {
    return createContiguous(&dirFile, fileName, size);
  }
  static void dateTimeCallback(void (*dateTime)(uint16_t& date, uint16_t& time)) {
    _oldDateTime = dateTime;
    _dateTime = dateTime ? oldToNew : 0;
  }
  uint8_t dirEntry(dir_t& dir) {return dirEntry(&dir);}
  uint8_t makeDir(SD_file& dir, const char* dirName) {return makeDir(&dir, dirName);}
  uint8_t open(SD_file& dirFile, const char* fileName, uint8_t oflag) {
    return open(&dirFile, fileName, oflag);
  }
  uint8_t open(SD_file& dirFile, const char* fileName) {
    return open(dirFile, fileName, O_RDWR);
  }
  uint8_t open(SD_file& dirFile, uint16_t index, uint8_t oflag) {
    return open(&dirFile, index, oflag);
  }
  uint8_t openRoot(SD_volume& vol) {return openRoot(&vol);}
  int8_t readDir(dir_t& dir) {return readDir(&dir);}
  static uint8_t remove(SD_file& dirFile, const char* fileName) {
    return remove(&dirFile, fileName);
  }
private:
  static void (*_oldDateTime)(uint16_t& date, uint16_t& time);
  static void oldToNew(uint16_t* date, uint16_t* time) {
    uint16_t d;
    uint16_t t;
    _oldDateTime(d, t);
    *date = d;
    *time = t;
  }
#endif  // ALLOW_DEPRECATED_FUNCTIONS
private:
  static uint8_t const F_OFLAG = (O_ACCMODE | O_APPEND | O_SYNC);
  static uint8_t const F_UNUSED = 0x30;
  static uint8_t const F_FILE_UNBUFFERED_READ = 0x40;
  static uint8_t const F_FILE_DIR_DIRTY = 0x80;
#if ((F_UNUSED | F_FILE_UNBUFFERED_READ | F_FILE_DIR_DIRTY) & F_OFLAG)
#error flags_ bits conflict
#endif  // flags_ bits
  uint8_t   _flags;         // See above for definition of flags_ bits
  uint8_t   _type;          // type of file see above for values
  uint32_t  _curCluster;    // cluster for current file position
  uint32_t  _curPosition;   // current file position in bytes from beginning
  uint32_t  _dirBlock;      // SD block that contains directory entry for file
  uint8_t   _dirIndex;      // index of entry in dirBlock 0 <= dirIndex_ <= 0XF
  uint32_t  _fileSize;      // file size in bytes
  uint32_t  _firstCluster;  // first cluster of file
  SD_volume* _vol;           // volume where file is located
  uint8_t addCluster(void);
  uint8_t addDirCluster(void);
  dir_t* cacheDirEntry(uint8_t action);
  static void (*_dateTime)(uint16_t* date, uint16_t* time);
  static uint8_t make83Name(const char* str, uint8_t* name);
  uint8_t openCachedEntry(uint8_t cacheIndex, uint8_t oflags);
  dir_t* readDirCache(void);
};

class SD_volume {
public:
  SD_volume(void) :_allocSearchStart(2), _fatType(0) {}
  static uint8_t* cacheClear(void) {
    cacheFlush();
    _cacheBlockNumber = 0xFFFFFFFF;
    return _cacheBuffer.data;
  }
  uint8_t init(SD_card* dev) { return init(dev, 1) ? true : init(dev, 0);}
  uint8_t init(SD_card* dev, uint8_t part);
  uint8_t blocksPerCluster(void) const {return _blocksPerCluster;}
  uint32_t blocksPerFat(void)  const {return _blocksPerFat;}
  uint32_t clusterCount(void) const {return _clusterCount;}
  uint8_t clusterSizeShift(void) const {return _clusterSizeShift;}
  uint32_t dataStartBlock(void) const {return _dataStartBlock;}
  uint8_t fatCount(void) const {return _fatCount;}
  uint32_t fatStartBlock(void) const {return _fatStartBlock;}
  uint8_t fatType(void) const {return _fatType;}
  uint32_t rootDirEntryCount(void) const {return _rootDirEntryCount;}
  uint32_t rootDirStart(void) const {return _rootDirStart;}
  static SD_card* sdCard(void) {return _sdCard;}
#if ALLOW_DEPRECATED_FUNCTIONS
  uint8_t init(SD_card& dev) {return init(&dev);}
  uint8_t init(SD_card& dev, uint8_t part) {return init(&dev, part);}
#endif  // ALLOW_DEPRECATED_FUNCTIONS
//------------------------------------------------------------------------------
  private:
  friend class SD_file;
  static uint8_t const CACHE_FOR_READ = 0;
  static uint8_t const CACHE_FOR_WRITE = 1;

  static cache_t _cacheBuffer;        // 512 byte cache for device blocks
  static uint32_t _cacheBlockNumber;  // Logical number of block in the cache
  static SD_card* _sdCard;            // SD_card object for cache
  static uint8_t _cacheDirty;         // cacheFlush() will write block if true
  static uint32_t _cacheMirrorBlock;  // block number for mirror FAT

  uint32_t _allocSearchStart;   // start cluster for alloc search
  uint8_t _blocksPerCluster;    // cluster size in blocks
  uint32_t _blocksPerFat;       // FAT size in blocks
  uint32_t _clusterCount;       // clusters in one FAT
  uint8_t _clusterSizeShift;    // shift to convert cluster count to block count
  uint32_t _dataStartBlock;     // first data block number
  uint8_t _fatCount;            // number of FATs on volume
  uint32_t _fatStartBlock;      // start block for first FAT
  uint8_t _fatType;             // volume type (12, 16, OR 32)
  uint16_t _rootDirEntryCount;  // number of entries in FAT16 root dir
  uint32_t _rootDirStart;       // root start block for FAT16, cluster for FAT32
  
  uint8_t allocContiguous(uint32_t count, uint32_t* curCluster);
  uint8_t blockOfCluster(uint32_t position) const {
    return (position >> 9) & (_blocksPerCluster - 1);
  }
  uint32_t clusterStartBlock(uint32_t cluster) const {
    return _dataStartBlock + ((cluster - 2) << _clusterSizeShift);
  }
  uint32_t blockNumber(uint32_t cluster, uint32_t position) const {
    return clusterStartBlock(cluster) + blockOfCluster(position);
  }
  static uint8_t cacheFlush(void);
  static uint8_t cacheRawBlock(uint32_t blockNumber, uint8_t action);
  static void cacheSetDirty(void) {_cacheDirty |= CACHE_FOR_WRITE;}
  static uint8_t cacheZeroBlock(uint32_t blockNumber);
  uint8_t chainSize(uint32_t beginCluster, uint32_t* size) const;
  uint8_t fatGet(uint32_t cluster, uint32_t* value) const;
  uint8_t fatPut(uint32_t cluster, uint32_t value);
  uint8_t fatPutEOC(uint32_t cluster) {
    return fatPut(cluster, 0x0FFFFFFF);
  }
  uint8_t freeChain(uint32_t cluster);
  uint8_t isEOC(uint32_t cluster) const {
    return  cluster >= (_fatType == 16 ? FAT16EOC_MIN : FAT32EOC_MIN);
  }
  uint8_t readBlock(uint32_t block, uint8_t* dst) {
    return _sdCard->readBlock(block, dst);}
  uint8_t readData(uint32_t block, uint16_t offset, uint16_t count, uint8_t* dst) {
      return _sdCard->readData(block, offset, count, dst);
  }
  uint8_t writeBlock(uint32_t block, const uint8_t* dst) {
    return _sdCard->writeBlock(block, dst);
  }
};
#endif // FAT_H
