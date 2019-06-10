// -----------------------------------------------------------------------------
// This file is part of vAmiga
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#import <Cocoa/Cocoa.h>
#import <MetalKit/MetalKit.h>

#import "va_std.h"

// Forward declarations
@class CPUProxy;
@class CIAProxy;
@class MemProxy;
@class AgnusProxy;
@class DeniseProxy;
@class PaulaProxy;
@class ControlPortProxy;
@class MouseProxy;
@class JoystickProxy;
@class KeyboardProxy;
@class DiskControllerProxy;
@class DriveProxy;
@class AmigaFileProxy;
@class ADFFileProxy;
@class AmigaSnapshotProxy;

/* Forward declarations of C++ class wrappers.
 * We wrap classes into normal C structs to avoid any reference to C++.
 */
struct AmigaWrapper;
struct CPUWrapper;
struct CIAWrapper;
struct MemWrapper;
struct AgnusWrapper;
struct DeniseWrapper;
struct PaulaWrapper;
struct AmigaControlPortWrapper;
struct KeyboardWrapper;
struct DiskControllerWrapper;
struct AmigaDriveWrapper;
struct AmigaFileWrapper;

//
// Amiga proxy
//

@interface AmigaProxy : NSObject {
    
    struct AmigaWrapper *wrapper;
    
    CPUProxy *cpu;
    CIAProxy *ciaA;
    CIAProxy *ciaB;
    MemProxy *mem;
    AgnusProxy *dma;
    DeniseProxy *denise;
    PaulaProxy *paula;
    ControlPortProxy *controlPort1;
    ControlPortProxy *controlPort2;
    MouseProxy *mouse;
    JoystickProxy *joystick1;
    JoystickProxy *joystick2;
    KeyboardProxy *keyboard;
    DiskControllerProxy *diskController;
    DriveProxy *df0;
    DriveProxy *df1;
    DriveProxy *df2;
    DriveProxy *df3;
}

@property (readonly) struct AmigaWrapper *wrapper;
@property (readonly) CPUProxy *cpu;
@property (readonly) CIAProxy *ciaA;
@property (readonly) CIAProxy *ciaB;
@property (readonly) MemProxy *mem;
@property (readonly) AgnusProxy *dma;
@property (readonly) DeniseProxy *denise;
@property (readonly) PaulaProxy *paula;
@property (readonly) ControlPortProxy *controlPort1;
@property (readonly) ControlPortProxy *controlPort2;
@property (readonly) MouseProxy *mouse;
@property (readonly) JoystickProxy *joystick1;
@property (readonly) JoystickProxy *joystick2;
@property (readonly) KeyboardProxy *keyboard;
@property (readonly) DiskControllerProxy *diskController;
@property (readonly) DriveProxy *df0;
@property (readonly) DriveProxy *df1;
@property (readonly) DriveProxy *df2;
@property (readonly) DriveProxy *df3;

// - (DriveProxy *)df:(NSInteger)n;

- (void) kill;

- (BOOL) releaseBuild;

- (uint64_t) masterClock;

- (void) setInspectionTarget:(EventID)id;
- (void) clearInspectionTarget;
- (BOOL) debugMode;
- (void) enableDebugging;
- (void) disableDebugging;
- (void) setDebugLevel:(NSInteger)value;

- (void) powerOn;
- (void) powerOff;
- (void) reset;
- (void) ping;
- (void) dump;

- (AmigaInfo) getInfo;

- (BOOL) readyToPowerUp;
- (BOOL) isPoweredOn;
- (BOOL) isPoweredOff;
- (BOOL) isRunning;
- (BOOL) isPaused;
- (void) run;
- (void) pause;

- (void) suspend;
- (void) resume;

- (AmigaConfiguration) config;
- (AmigaMemConfiguration) memConfig;
- (BOOL) configure:(ConfigOption)option value:(NSInteger)value;
- (BOOL) configureModel:(NSInteger)model;
- (BOOL) configureLayout:(NSInteger)value;
- (BOOL) configureChipMemory:(NSInteger)size;
- (BOOL) configureSlowMemory:(NSInteger)size;
- (BOOL) configureFastMemory:(NSInteger)size;
- (void) configureRealTimeClock:(BOOL)value;
- (BOOL) configureDrive:(NSInteger)nr connected:(BOOL)value;
- (BOOL) configureDrive:(NSInteger)nr type:(NSInteger)value;
- (BOOL) configureDrive:(NSInteger)nr speed:(NSInteger)value;
- (void) configureExactBlitter:(BOOL)value;
- (void) configureFifoBuffering:(BOOL)value;

// Message queue
- (void) addListener:(const void *)sender function:(Callback *)func;
- (void) removeListener:(const void *)sender;
- (Message)message;

- (void) stopAndGo;
- (void) stepInto;
- (void) stepOver;

- (BOOL) warp;
- (void) setWarp:(BOOL)value;

// Handling snapshots
- (BOOL) takeAutoSnapshots;
- (void) setTakeAutoSnapshots:(BOOL)b;
- (void) suspendAutoSnapshots;
- (void) resumeAutoSnapshots;
- (NSInteger) snapshotInterval;
- (void) setSnapshotInterval:(NSInteger)value;
 
- (void) loadFromSnapshot:(AmigaSnapshotProxy *)proxy;

- (BOOL) restoreAutoSnapshot:(NSInteger)nr;
- (BOOL) restoreUserSnapshot:(NSInteger)nr;
- (BOOL) restoreLatestAutoSnapshot;
- (BOOL) restoreLatestUserSnapshot;
- (NSInteger) numAutoSnapshots;
- (NSInteger) numUserSnapshots;

- (NSData *) autoSnapshotData:(NSInteger)nr;
- (NSData *) userSnapshotData:(NSInteger)nr;
- (unsigned char *) autoSnapshotImageData:(NSInteger)nr;
- (unsigned char *) userSnapshotImageData:(NSInteger)nr;
- (NSSize) autoSnapshotImageSize:(NSInteger)nr;
- (NSSize) userSnapshotImageSize:(NSInteger)nr;
- (time_t) autoSnapshotTimestamp:(NSInteger)nr;
- (time_t) userSnapshotTimestamp:(NSInteger)nr;

- (void) takeUserSnapshot;

- (void) deleteAutoSnapshot:(NSInteger)nr;
- (void) deleteUserSnapshot:(NSInteger)nr;

@end


//
// CPU Proxy
//

@interface CPUProxy : NSObject {
    
    struct CPUWrapper *wrapper;
}

- (void) dump;
- (CPUInfo) getInfo;
- (DisassembledInstruction) getInstrInfo:(NSInteger)index;
- (DisassembledInstruction) getTracedInstrInfo:(NSInteger)index;

- (BOOL) hasBreakpointAt:(uint32_t)addr;
- (BOOL) hasDisabledBreakpointAt:(uint32_t)addr;
- (BOOL) hasConditionalBreakpointAt:(uint32_t)addr;
- (void) setBreakpointAt:(uint32_t)addr;
- (void) deleteBreakpointAt:(uint32_t)addr;
- (void) enableBreakpointAt:(uint32_t)addr;
- (void) disableBreakpointAt:(uint32_t)addr;

- (NSInteger) traceBufferCapacity;
- (void) truncateTraceBuffer:(NSInteger)count;

- (NSInteger) numberOfBreakpoints;
- (void) deleteBreakpoint:(NSInteger)nr; 
- (BOOL) isDisabled:(NSInteger)nr;
- (BOOL) hasCondition:(NSInteger)nr;
- (BOOL) hasSyntaxError:(NSInteger)nr;
- (uint32_t) breakpointAddr:(NSInteger)nr;
- (BOOL) setBreakpointAddr:(NSInteger)nr addr:(uint32_t)addr;
- (NSString *) breakpointCondition:(NSInteger)nr;
- (BOOL) setBreakpointCondition:(NSInteger)nr cond:(NSString *)cond;

@end


//
// CIA Proxy
//

@interface CIAProxy : NSObject {
    
    struct CIAWrapper *wrapper;
}

- (void) dump;
- (CIAInfo) getInfo;
- (void) poke:(uint16_t)addr value:(uint8_t)value;

@end


//
// Memory Proxy
//

@interface MemProxy : NSObject {
    
    struct MemWrapper *wrapper;
}

- (void) dump;

- (BOOL) hasBootRom;
- (void) deleteBootRom;
- (BOOL) isBootRom:(NSURL *)url;
- (BOOL) loadBootRomFromBuffer:(NSData *)buffer;
- (BOOL) loadBootRomFromFile:(NSURL *)url;
- (uint64_t) bootRomFingerprint;

- (BOOL) hasKickRom;
- (void) deleteKickRom;
- (BOOL) isKickRom:(NSURL *)url;
- (BOOL) loadKickRomFromBuffer:(NSData *)buffer;
- (BOOL) loadKickRomFromFile:(NSURL *)url;
- (uint64_t) kickRomFingerprint;

- (MemorySource *) getMemSrcTable; 
- (MemorySource) memSrc:(NSInteger)addr;
- (NSInteger) spypeek8:(NSInteger)addr;
- (NSInteger) spypeek16:(NSInteger)addr;
- (void) poke8:(NSInteger)addr value:(NSInteger)value;
- (void) poke16:(NSInteger)addr value:(NSInteger)value;
- (void) pokeCustom16:(NSInteger)addr value:(NSInteger)value;

- (NSString *) ascii:(NSInteger)addr;
- (NSString *) hex:(NSInteger)addr bytes:(NSInteger)bytes;

@end


//
// AgnusProxy Proxy
//

@interface AgnusProxy : NSObject {
    
    struct AgnusWrapper *wrapper;
}

- (void) dump;
- (void) dumpEventHandler;
- (void) dumpCopper;
- (void) dumpBlitter;

- (DMAInfo) getInfo;
- (DMADebuggerInfo) getDebuggerInfo;
- (NSInteger) primSlotCount;
- (NSInteger) secSlotCount;
- (EventSlotInfo) getPrimarySlotInfo:(NSInteger)slot;
- (EventSlotInfo) getSecondarySlotInfo:(NSInteger)slot;
- (EventHandlerInfo) getEventInfo;
- (CopperInfo) getCopperInfo;
- (BlitterInfo) getBlitterInfo;

- (BOOL) isIllegalInstr:(NSInteger)addr;
- (NSString *) disassemble:(NSInteger)addr;
- (NSString *) disassemble:(NSInteger)list instr:(NSInteger)offset;

- (void) dmaDebugSetEnable:(BOOL)value;
- (void) dmaDebugSetVisualize:(BusOwner)owner value:(BOOL)value;
- (void) dmaDebugSetColor:(BusOwner)owner r:(double)r g:(double)g b:(double)b;
- (void) dmaDebugSetOpacity:(double)value;

@end


//
// Denise Proxy
//

@interface DeniseProxy : NSObject {
    
    struct DeniseWrapper *wrapper;
}

- (void) dump;
- (DeniseInfo) getInfo;
- (SpriteInfo) getSpriteInfo:(NSInteger)nr;
- (void) inspect;

- (void) pokeColorReg:(NSInteger)reg value:(UInt16)value;

- (double) palette;
- (void) setPalette:(Palette)p;
- (double) brightness;
- (void) setBrightness:(double)value;
- (double) saturation;
- (void) setSaturation:(double)value;
- (double) contrast;
- (void) setContrast:(double)value;

- (void) setBPU:(NSInteger)count;
- (void) setBPLCONx:(NSInteger)x value:(NSInteger)value;
- (void) setBPLCONx:(NSInteger)x bit:(NSInteger)bit value:(BOOL)value;
- (void) setBPLCONx:(NSInteger)x nibble:(NSInteger)nibble value:(NSInteger)value;

- (ScreenBuffer) stableLongFrame;
- (ScreenBuffer) stableShortFrame;

- (BOOL) interlaceMode;
- (BOOL) isLongFrame;
- (BOOL) isShortFrame;

@end


//
// Paula Proxy
//

@interface PaulaProxy : NSObject {
    
    struct PaulaWrapper *wrapper;
}

- (void) dump;
- (PaulaInfo) getInfo;
- (AudioInfo) getAudioInfo;
- (DiskControllerInfo) getDiskControllerInfo;

- (uint32_t) sampleRate;
- (void) setSampleRate:(double)rate;

- (NSInteger) ringbufferSize;
- (float) ringbufferDataL:(NSInteger)offset;
- (float) ringbufferDataR:(NSInteger)offset;
- (float) ringbufferData:(NSInteger)offset;
- (double) fillLevel;
- (NSInteger) bufferUnderflows;
- (NSInteger) bufferOverflows;

- (void) readMonoSamples:(float *)target size:(NSInteger)n;
- (void) readStereoSamples:(float *)target1 buffer2:(float *)target2 size:(NSInteger)n;
- (void) readStereoSamplesInterleaved:(float *)target size:(NSInteger)n;

- (void) rampUp;
- (void) rampUpFromZero;
- (void) rampDown;

@end


//
// ControlPort Proxy
//

@interface ControlPortProxy : NSObject {
    
    struct AmigaControlPortWrapper *wrapper;
}

- (void) dump;

- (void) connectDevice:(ControlPortDevice)value;

// - (void) connectMouse:(BOOL)value;

@end


//
// Mouse Proxy
//

@interface MouseProxy : NSObject {
    
    struct MouseWrapper *wrapper;
}

- (void) dump;

- (void) setXY:(NSPoint)pos;
- (void) setLeftButton:(BOOL)value;
- (void) setRightButton:(BOOL)value;

@end


//
// Joystick Proxy
//

@interface JoystickProxy : NSObject {
    
    struct JoystickWrapper *wrapper;
}

- (void) dump;

- (void) trigger:(JoystickEvent)event;
- (BOOL) autofire;
- (void) setAutofire:(BOOL)value;
- (NSInteger) autofireBullets;
- (void) setAutofireBullets:(NSInteger)value;
- (float) autofireFrequency;
- (void) setAutofireFrequency:(float)value;

@end


//
// Keyboard Proxy
//

@interface KeyboardProxy : NSObject {
    
    struct KeyboardWrapper *wrapper;
}

- (void) dump;

- (BOOL) keyIsPressed:(NSInteger)keycode;
- (void) pressKey:(NSInteger)keycode;
- (void) releaseKey:(NSInteger)keycode;
- (void) releaseAllKeys;

@end


//
// DiskController Proxy
//

@interface DiskControllerProxy : NSObject {
    
    struct DiskControllerWrapper *wrapper;
}

- (void) dump;

- (BOOL) spinning:(NSInteger)df;
- (BOOL) spinning;

- (BOOL) isConnected:(NSInteger)df;
- (void) setConnected:(NSInteger)df value:(BOOL)value;
- (void) toggleConnected:(NSInteger)df;

// - (BOOL) doesDMA:(NSInteger)nr;

@end


//
// AmigaDrive Proxy
//

@interface DriveProxy : NSObject {
    
    struct AmigaDriveWrapper *wrapper;
}

@property (readonly) struct AmigaDriveWrapper *wrapper;

- (void) dump;

- (NSInteger) nr;
- (DriveType) type;

- (BOOL) hasDisk;
- (BOOL) hasWriteProtectedDisk;
- (void) setWriteProtection:(BOOL)value;
- (void) toggleWriteProtection;

- (BOOL) hasModifiedDisk;
- (void) setModifiedDisk:(BOOL)value;

- (void) ejectDisk;
- (void) insertDisk:(ADFFileProxy *)file;

- (ADFFileProxy *)convertDisk;

@end


//
// F I L E   T Y P E   P R O X Y S
//

//
// AmigaFile proxy
//

@interface AmigaFileProxy : NSObject {
    
    struct AmigaFileWrapper *wrapper;
}

- (struct AmigaFileWrapper *)wrapper;

- (AmigaFileType)type;
- (void)setPath:(NSString *)path;
- (NSInteger)sizeOnDisk;

- (void)seek:(NSInteger)offset;
- (NSInteger)read;

- (void)readFromBuffer:(const void *)buffer length:(NSInteger)length;
- (NSInteger)writeToBuffer:(void *)buffer;

@end


//
// Snapshot proxy
//

@interface AmigaSnapshotProxy : AmigaFileProxy {
}

+ (BOOL)isSupportedSnapshot:(const void *)buffer length:(NSInteger)length;
+ (BOOL)isUnsupportedSnapshot:(const void *)buffer length:(NSInteger)length;
+ (BOOL)isSupportedSnapshotFile:(NSString *)path;
+ (BOOL)isUnsupportedSnapshotFile:(NSString *)path;
+ (instancetype)makeWithBuffer:(const void *)buffer length:(NSInteger)length;
+ (instancetype)makeWithFile:(NSString *)path;
+ (instancetype)makeWithAmiga:(AmigaProxy *)amiga;

@end


//
// ADFFile proxy
//

@interface ADFFileProxy : AmigaFileProxy {
}

+ (BOOL)isADFFile:(NSString *)path;
+ (instancetype)makeWithBuffer:(const void *)buffer length:(NSInteger)length;
+ (instancetype)makeWithFile:(NSString *)path;
+ (instancetype)makeWithDiskType:(DiskType)type;
+ (instancetype)makeWithDrive:(DriveProxy *)drive;

- (DiskType)diskType;
- (NSInteger)numCylinders;
- (NSInteger)numHeads;
- (NSInteger)numTracks;
- (NSInteger)numSectors;
- (NSInteger)numSectorsPerTrack;
- (void)formatDisk:(FileSystemType)fs;
- (void)seekTrack:(NSInteger)nr;
- (void)seekSector:(NSInteger)nr;

@end

