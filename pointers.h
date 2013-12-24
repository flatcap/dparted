/* Copyright (c) 2013 Richard Russon (FlatCap)
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Library General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef _POINTERS_H_
#define _POINTERS_H_

#include <memory>

//XXX break up somewhat

class App;
class Block;
class Disk;
class Container;
class Extended;
class File;
class Filesystem;
class Gpt;
class Loop;
class LvmGroup;
class LvmLinear;
class LvmMetadata;
class LvmMirror;
class LvmPartition;
class LvmRaid;
class LvmStripe;
class LvmTable;
class LvmVolume;
class MdGroup;
class MdTable;
class Misc;
class Msdos;
class Partition;
class Table;
class Volume;
class Whole;

typedef std::shared_ptr<App>          AppPtr;
typedef std::shared_ptr<Block>        BlockPtr;
typedef std::shared_ptr<Disk>         DiskPtr;
typedef std::shared_ptr<Container>  ContainerPtr;
typedef std::shared_ptr<Extended>     ExtendedPtr;
typedef std::shared_ptr<File>         FilePtr;
typedef std::shared_ptr<Filesystem>   FilesystemPtr;
typedef std::shared_ptr<Gpt>          GptPtr;
typedef std::shared_ptr<Loop>         LoopPtr;
typedef std::shared_ptr<LvmGroup>     LvmGroupPtr;
typedef std::shared_ptr<LvmLinear>    LvmLinearPtr;
typedef std::shared_ptr<LvmMetadata>  LvmMetadataPtr;
typedef std::shared_ptr<LvmMirror>    LvmMirrorPtr;
typedef std::shared_ptr<LvmPartition> LvmPartitionPtr;
typedef std::shared_ptr<LvmRaid>      LvmRaidPtr;
typedef std::shared_ptr<LvmStripe>    LvmStripePtr;
typedef std::shared_ptr<LvmTable>     LvmTablePtr;
typedef std::shared_ptr<LvmVolume>    LvmVolumePtr;
typedef std::shared_ptr<MdGroup>      MdGroupPtr;
typedef std::shared_ptr<MdTable>      MdTablePtr;
typedef std::shared_ptr<Misc>         MiscPtr;
typedef std::shared_ptr<Msdos>        MsdosPtr;
typedef std::shared_ptr<Partition>    PartitionPtr;
typedef std::shared_ptr<Table>        TablePtr;
typedef std::shared_ptr<Volume>       VolumePtr;
typedef std::shared_ptr<Whole>        WholePtr;

#endif // _POINTERS_H_

