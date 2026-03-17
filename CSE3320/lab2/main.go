package main

import (
	"fmt"
	"os"
	"strconv"
	"encoding/binary"
	"bytes"
	"time"
)

const BlockSize = 256

var currentUser string = "default"
var currentDisk = "disk01"

type Superblock struct {
	TotalBlocks int32 // Total size of the disk blocks
	FNTStart int32 // Where the filename table begins
	FNTBlocks int32 // How many blocks are reserved for filenames
	DABPTStart int32 // Start of the file attributes
	DABPTBlocks int32 // How many blocks reserved for file metadata
	BPTStart int32 // Start of block pointer table
	BPTBlocks int32 // How many blocks reserved for pointers
	DataStart int32 // First block available for actual file content
}

type FNTEntry struct {
	Filename [56]byte
	DABPTIndex int32
}

type DABPTEntry struct {
	FileSize   int32
	Timestamp  int64
	BPTIndex   int32
	Username   [40]byte
	RefCount   int32
	Padding    [4]byte
}

type BPTEntry struct {
	Direct [8]int32
}

func check(err error, msg string) {
	if err != nil {
		fmt.Println(msg, err)
		os.Exit(1)
	}
}

// Create a 256,000 byte file dividded into 1000 blocks of 256 bytes each
func createFS(numBlocks int, filename string) error {
	size := int64(numBlocks * BlockSize)

	file, err := os.Create(filename)
	if err != nil {
		return err
	}
	defer file.Close()

	err = file.Truncate(size)
	if err != nil {
		return err
	}
	fmt.Println("Filesystem created:", filename)
	fmt.Println("Total size:", size, "bytes")
	return nil
}

// Write superblock and allow dynamic sizing
func formatFS(file *os.File, totalBlocks int32, fntEntries int32, dabptEntries int32) error {

	fntBlocks := (fntEntries + 3) / 4
	dabptBlocks := (dabptEntries + 3) / 4
	bptBlocks := int32(20)
	dataStart := int32(2) + int32(fntBlocks) + int32(dabptBlocks) + int32(bptBlocks)

	sb := Superblock{
		TotalBlocks: totalBlocks,
		FNTStart: 2,
		FNTBlocks: fntBlocks,
		DABPTStart: 2 + fntBlocks,
		DABPTBlocks: dabptBlocks,
		BPTStart: 2 + fntBlocks + dabptBlocks,
		BPTBlocks: bptBlocks,
		DataStart: dataStart,
	}

	err := writeSuperblock(file, sb)
	if err != nil {
		return err
	}

	return initializeBitmap(file, dataStart)
}

// Finds first free bit in bitmap, set it, and return block num
func allocateBlock(file *os.File) (int32, error) {
	bitmap, err := readBlock(file, 1)
	if err != nil {
		return -1, err
	}

	sb, err := readSuperblock(file)
	if err != nil {
		return -1, err
	}

	for i := int32(0); i < sb.TotalBlocks; i++ {
		byteIndex := i/8
		bitOffset := i%8
		if bitmap[byteIndex]&(1<<bitOffset) == 0 {
			bitmap[byteIndex] |= (1 << bitOffset)

			err = writeBlock(file, 1, bitmap)
			if err != nil {
				return -1, err
			}

			return i, nil
		}
	}

	return -1, fmt.Errorf("no free blocks available")
}

//Clear bit in bitmap
func freeBlock(file *os.File, blockNum int32) error {
	bitmap, err := readBlock(file, 1)
	if err != nil {
		return err
	}

	byteIndex := blockNum / 8
	bitOffset := blockNum % 8
	bitmap[byteIndex] &^= (1 << bitOffset)

	return writeBlock(file, 1, bitmap)
}

func writeBlock(file *os.File, blockNum int, data []byte) error {
	if len(data) > BlockSize {
		return fmt.Errorf("data too large for block")
	}

	offset := int64(blockNum * BlockSize)
	_, err := file.Seek(offset, 0)
	if err != nil {
		return err
	}

	_, err = file.Write(data)
	return err
}

func readBlock(file *os.File, blockNum int) ([]byte, error) {
	offset := int64(blockNum * BlockSize)
	_, err := file.Seek(offset, 0)
	if err != nil {
		return nil, err
	}

	buffer := make([]byte, BlockSize)
	_, err = file.Read(buffer)
	if err != nil {
		return nil, err
	}

	return buffer, nil
}

func writeSuperblock(file *os.File, sb Superblock) error {
	buffer := new(bytes.Buffer)
	err := binary.Write(buffer, binary.LittleEndian, sb)
	if err != nil {
		return err
	}
	data := buffer.Bytes()
	padded := make([]byte, BlockSize)
	copy(padded, data)

	return writeBlock(file, 0, padded)
}

func readSuperblock(file  *os.File) (Superblock, error) {
	data, err := readBlock(file, 0)
	if err != nil {
		return Superblock{}, err
	}

	var sb Superblock
	buffer := bytes.NewReader(data)
	err = binary.Read(buffer, binary.LittleEndian, &sb)
	return sb, err
}

func initializeBitmap(file *os.File, reservedUntil int32) error {
	bitmap := make([]byte, BlockSize)

	for i := int32(0); i < reservedUntil; i++ {
		byteIndex := i / 8
		bitOffset := i % 8
		bitmap[byteIndex] |= (1 << bitOffset)
	}

	return writeBlock(file, 1, bitmap)
}

// Finds first empty FNT block then writes to
func writeFNTEntry(file *os.File, sb Superblock, entry FNTEntry) error {
	for block := sb.FNTStart; block < sb.FNTStart+sb.FNTBlocks; block++ {
		data, err := readBlock(file, int(block))
		if err != nil {
			return err
		}

		buffer := bytes.NewBuffer(data)

		for i := 0; i < 4; i++ {
			var existing FNTEntry
			err = binary.Read(buffer, binary.LittleEndian, &existing)
			if err != nil {
				return err
			}

			if existing.DABPTIndex == 0 {
				offset := int64(block)*int64(BlockSize) + int64(i*60)
				_, err = file.Seek(offset, 0)
				if err != nil {
					return err
				}
				return binary.Write(file, binary.LittleEndian, entry)
			}
		}
	}
	return fmt.Errorf("no free FNT entries")
}

// Prints all non zero filenames from FNT
func listFiles(file *os.File) error {
	sb, err := readSuperblock(file)
	if err != nil {
		return err
	}

	fmt.Println("Files in FS:")
	for block := sb.FNTStart; block < sb.FNTStart+sb.FNTBlocks; block++ {
		data, err := readBlock(file, int(block))
		if err != nil {
			return err
		}

		buffer := bytes.NewBuffer(data)
		for i := 0; i < 4; i++ {
			var entry FNTEntry
			err = binary.Read(buffer, binary.LittleEndian, &entry)
			if err != nil {
				return err
			}

			if entry.DABPTIndex != 0 {
				name := string(bytes.Trim(entry.Filename[:], "\x00"))

				dabpt, err := readDABPTEntry(file, sb, entry.DABPTIndex)
				if err != nil {
					return err
				}

				user := string(bytes.Trim(dabpt.Username[:], "\x00"))
				timeStr := time.Unix(dabpt.Timestamp, 0).Format("2006-01-02 15:04:05")

				fmt.Printf("- %s (size: %d bytes, user: %s, last mod: %s)\n", name, dabpt.FileSize, user, timeStr)
			}
		}
	}
	return nil
}

// Find empty DABPT slot, write to, return index
func writeDABPTEntry(file *os.File, sb Superblock, entry DABPTEntry) (int32, error) {
	for block := sb.DABPTStart; block < sb.DABPTStart+sb.DABPTBlocks; block++ {
		data, err := readBlock(file, int(block))
		if err != nil {
			return -1, err
		}

		buffer := bytes.NewBuffer(data)
		for i := 0; i < 4; i++ {
			var existing DABPTEntry
			err = binary.Read(buffer, binary.LittleEndian, &existing)
			if err != nil {
				return -1, err
			}
			if existing.FileSize == 0 {
				offset := int64(block)*int64(BlockSize) + int64(i*64)

				_, err = file.Seek(offset, 0)
				if err != nil {
					return -1, err
				}

				err = binary.Write(file, binary.LittleEndian, entry)
				if err != nil {
					return -1, err
				}

				index := (block - sb.DABPTStart)*4 + int32(i) + 1
				return index, nil
			}
		}
	}
	return -1, fmt.Errorf("no free DABPT entries")
}

// Find empty BPT slot, write to, then return index
func writeBPTEntry(file *os.File, sb Superblock, entry BPTEntry) (int32, error) {
	for block := sb.BPTStart; block < sb.BPTStart+sb.BPTBlocks; block++ {
		data, err := readBlock(file, int(block))
		if err != nil {
			return -1, err
		}
		buffer := bytes.NewBuffer(data)
		for i := 0; i < 8; i++ {
			var existing BPTEntry
			err = binary.Read(buffer, binary.LittleEndian, &existing)
			if err != nil {
				return -1, err
			}

			if existing.Direct[0] == 0 {
				offset := int64(block)*int64(BlockSize) + int64(i*32)

				_, err = file.Seek(offset, 0)
				if err != nil {
					return -1, err
				}

				err = binary.Write(file, binary.LittleEndian, entry)
				if err != nil {
					return -1, err
				}

				index := (block - sb.BPTStart)*8 + int32(i) + 1
				return index, nil
			}
		}
	}
	return -1, fmt.Errorf("no free BPT entries")
}

func readDABPTEntry(file *os.File, sb Superblock, index int32) (DABPTEntry, error) {
    if index <= 0 || index > sb.DABPTBlocks*4 {
        return DABPTEntry{}, fmt.Errorf("invalid DABPT index: %d (must be 1..%d)", index, sb.DABPTBlocks*4)
    }
    index--
	block := sb.DABPTStart + index/4
	offsetInBlock := (index % 4) * 64

	data, err := readBlock(file, int(block))
	if err != nil {
		return DABPTEntry{}, err
	}

	var entry DABPTEntry
	buffer := bytes.NewReader(data[offsetInBlock : offsetInBlock+64])
	err = binary.Read(buffer, binary.LittleEndian, &entry)
	return entry, err
}

func readBPTEntry(file *os.File, sb Superblock, index int32) (BPTEntry, error) {
    if index <= 0 || index > sb.BPTBlocks*8 {
        return BPTEntry{}, fmt.Errorf("invalid BPT index: %d (must be 1..%d)", index, sb.BPTBlocks*8)
    }
    index--
    block := sb.BPTStart + index/8
    offsetInBlock := (index % 8) * 32

    data, err := readBlock(file, int(block))
    if err != nil {
        return BPTEntry{}, err
    }

    var entry BPTEntry
    buffer := bytes.NewReader(data[offsetInBlock : offsetInBlock+32])
    err = binary.Read(buffer, binary.LittleEndian, &entry)
    return entry, err
}

// Scan all FNT entries looking for matching file
func findFNTEntry(file *os.File, sb Superblock, name string) (FNTEntry, int64, error) {
	for block := sb.FNTStart; block < sb.FNTStart+sb.FNTBlocks; block++ {
		data, _ := readBlock(file, int(block))
		buffer := bytes.NewBuffer(data)

		for i := 0; i < 4; i++ {
			var entry FNTEntry
			binary.Read(buffer, binary.LittleEndian, &entry)

			filename := string(bytes.Trim(entry.Filename[:], "\x00"))
			if filename == name {
				offset := int64(block)*BlockSize + int64(i*60)
				return entry, offset, nil
			}
		}
	}
	return FNTEntry{}, 0, fmt.Errorf("file not found")
}

func main() {
	if len(os.Args) < 2 {
		fmt.Println("Invalid args")
		return
	}

	command := os.Args[1]
	if command == "Createfs" {
		if len(os.Args) != 3 {
			fmt.Println("Usage: Createfs <numBlocks>")
			return
		}

		numBlocks, err := strconv.Atoi(os.Args[2])
		if err != nil {
			fmt.Println("Invalid number")
			return
		}

		err = createFS(numBlocks, "disk01")
		if err != nil {
			fmt.Println("Error:", err)
			return
		}

		file, err := os.OpenFile("disk01", os.O_RDWR, 0666)
		if err != nil {
			fmt.Println("error opening disk:", err)
			return
		}
		defer file.Close()

		const DefaultFilenames = 40
		const DefaultDABPTEntries = 40

		err = formatFS(file, int32(numBlocks), DefaultFilenames, DefaultDABPTEntries)
		if err != nil {
			fmt.Println("Error formatting:", err)
			return
		}

		fmt.Println("Filesystem created and formatted with defaults.")
		fmt.Printf("   FNT: %d entries (%d blocks)\n", DefaultFilenames, (DefaultFilenames+3)/4)
		fmt.Printf("   DABPT: %d entries (%d blocks)\n", DefaultDABPTEntries, (DefaultDABPTEntries+3)/4)
	}

	if command == "Formatfs" {
		if len(os.Args) != 4 {
			fmt.Println("Usage: Formatfs #filenames #DABPTentries")
			return
		}

		fntCount, _ := strconv.Atoi(os.Args[2])
		dabptCount, _ := strconv.Atoi(os.Args[3])

		file, _ := os.OpenFile("disk01", os.O_RDWR, 0666)
		defer file.Close()

		sb, _ := readSuperblock(file)

		err := formatFS(file, sb.TotalBlocks, int32(fntCount), int32(dabptCount))
		if err != nil {
			fmt.Println("Format error:", err)
			return
		}

		fmt.Println("Filesystem formatted.")
	}

	if command == "List" {
		file, err := os.OpenFile("disk01", os.O_RDWR, 0666)
		if err != nil {
			fmt.Println("error opening disk:", err)
			return
		}
		defer file.Close()

		err = listFiles(file)
		if err != nil {
			fmt.Println("Error listing files:", err)
		}
	}

	if command == "Put" {
		if len(os.Args) != 3 {
			fmt.Println("Usage: Put <externalFile>")
			return
		}

		hostFile := os.Args[2]
		if len(hostFile) > 55 {
			fmt.Println("Error: Filename too long (max 55 chars)")
			return
		}

		file, err := os.OpenFile("disk01", os.O_RDWR, 0666)
		if err != nil {
			fmt.Println("Error opening disk:", err)
			return
		}
		defer file.Close()

		sb, err := readSuperblock(file)
		if err != nil {
			fmt.Println("Error reading superblock:", err)
			return
		}

		_, _, err = findFNTEntry(file, sb, hostFile)
		if err == nil {
			fmt.Println("Error: File already exists in filesystem")
			return
		}

		inputData, err := os.ReadFile(hostFile)
		if err != nil {
			fmt.Println("Error reading external file:", err)
			return
		}

		var bpt BPTEntry
		var blockCount int

		for i := 0; i < len(inputData); i += BlockSize {
			end := i + BlockSize
			if end > len(inputData) {
				end = len(inputData)
			}

			blockNum, err := allocateBlock(file)
			check(err, "Error allocating block:")

			err = writeBlock(file, int(blockNum), inputData[i:end])
			check(err, "Error writing data block:")

			bpt.Direct[blockCount] = blockNum
			blockCount++
		}

		bptIndex, err := writeBPTEntry(file, sb, bpt)
		check(err, "Error writing BPT entry:")

		var dabpt DABPTEntry
		dabpt.FileSize = int32(len(inputData))
		dabpt.Timestamp = time.Now().Unix()
		dabpt.BPTIndex = bptIndex
		dabpt.RefCount = 1
		copy(dabpt.Username[:], []byte(currentUser))

		dabptIndex, err := writeDABPTEntry(file, sb, dabpt)
		check(err, "Error writing DABPT entry:")

		var entry FNTEntry
		var fname [56]byte
		copy(fname[:], []byte(hostFile))
		entry.Filename = fname
		entry.DABPTIndex = dabptIndex

		err = writeFNTEntry(file, sb, entry)
		check(err, "Error writing FNT entry:")

		fmt.Println("File stored successfully.")
	}

	if command == "Get" {
		if len(os.Args) != 3 {
			fmt.Println("Usage: Get <filename>")
			return
		}

		filename := os.Args[2]

		file, _ := os.OpenFile("disk01", os.O_RDWR, 0666)
		defer file.Close()

		sb, _ := readSuperblock(file)

		fnt, _, err := findFNTEntry(file, sb, filename)
		if err != nil {
			fmt.Println("File not found")
			return
		}

		dabpt, _ := readDABPTEntry(file, sb, fnt.DABPTIndex)
		bpt, _ := readBPTEntry(file, sb, dabpt.BPTIndex)

		output := []byte{}

		for _, block := range bpt.Direct {
			if block == 0 {
				break
			}
			data, _ := readBlock(file, int(block))
			output = append(output, data...)
		}

		os.WriteFile("output_"+filename, output[:dabpt.FileSize], 0644)

		fmt.Println("File retrieved successfully.")
	}

	if command == "Remove" {
		if len(os.Args) != 3 {
			fmt.Println("Usage: Remove <filename>")
			return
		}

		filename := os.Args[2]

		file, err := os.OpenFile("disk01", os.O_RDWR, 0666)
		if err != nil {
			fmt.Println("Error opening disk:", err)
			return
		}
		defer file.Close()

		sb, err := readSuperblock(file)
		if err != nil {
			fmt.Println("Error reading superblock:", err)
			return
		}

		fnt, fntOffset, err := findFNTEntry(file, sb, filename)
		if err != nil {
			fmt.Println("File not found")
			return
		}

		dabpt, err := readDABPTEntry(file, sb, fnt.DABPTIndex)
		if err != nil {
			fmt.Println("Error reading DABPT:", err)
			return
		}

		bpt, err := readBPTEntry(file, sb, dabpt.BPTIndex)
		if err != nil {
			fmt.Println("Error reading BPT:", err)
			return
		}

		for _, block := range bpt.Direct {
			if block != 0 {
				freeBlock(file, block)
			}
		}

		emptyDABPT := DABPTEntry{}
		dabptBlock := sb.DABPTStart + (fnt.DABPTIndex-1)/4
		dabptOffsetInBlock := ((fnt.DABPTIndex - 1) % 4) * 64
		dabptAbsOffset := int64(dabptBlock)*BlockSize + int64(dabptOffsetInBlock)

		file.Seek(dabptAbsOffset, 0)
		binary.Write(file, binary.LittleEndian, emptyDABPT)

		emptyBPT := BPTEntry{}
		bptBlock := sb.BPTStart + (dabpt.BPTIndex-1)/8
		bptOffsetInBlock := ((dabpt.BPTIndex - 1) % 8) * 32
		bptAbsOffset := int64(bptBlock)*BlockSize + int64(bptOffsetInBlock)

		file.Seek(bptAbsOffset, 0)
		binary.Write(file, binary.LittleEndian, emptyBPT)

		file.Seek(fntOffset, 0)
		emptyFNT := FNTEntry{}
		binary.Write(file, binary.LittleEndian, emptyFNT)

		fmt.Println("File removed.")
	}

	if command == "Savefs" {
		if len(os.Args) != 3 {
			fmt.Println("Usage: Savefs name")
			return
		}

		input, _ := os.ReadFile("disk01")
		os.WriteFile(os.Args[2], input, 0644)
		fmt.Println("Filesystem saved.")
	}

	if command == "Openfs" {
		if len(os.Args) != 3 {
			fmt.Println("Usage: Openfs name")
			return
		}

		input, err := os.ReadFile(os.Args[2])
		if err != nil {
			fmt.Println("Error:", err)
			return
		}

		currentDisk = "disk01"
		os.WriteFile(currentDisk, input, 0644)
		fmt.Println("Filesystem opened.")
	}

	if command == "Rename" {
		if len(os.Args) != 4 {
			fmt.Println("Usage: Rename oldname newname")
			return
		}

		oldname := os.Args[2]
		newname := os.Args[3]

		if len(newname) > 55 {
			fmt.Println("Error: New filename too long (max 55 chars)")
			return
		}

		file, err := os.OpenFile("disk01", os.O_RDWR, 0666)
		if err != nil {
			fmt.Println("Error opening disk:", err)
			return
		}
		defer file.Close()

		sb, err := readSuperblock(file)
		if err != nil {
			fmt.Println("Error reading superblock:", err)
			return
		}

		fnt, offset, err := findFNTEntry(file, sb, oldname)
		if err != nil {
			fmt.Println("File not found")
			return
		}

		var newFilename [56]byte
		copy(newFilename[:], []byte(newname))

		fnt.Filename = newFilename

		file.Seek(offset, 0)
		err = binary.Write(file, binary.LittleEndian, fnt)
		if err != nil {
			fmt.Println("Error writing renamed entry:", err)
			return
		}

		fmt.Println("File renamed.")
	}

	if command == "User" {
		if len(os.Args) != 3 {
			fmt.Println("Usage: User name")
			return
		}
		currentUser = os.Args[2]
		fmt.Println("Current user set to", currentUser)
	}

	if command == "Link" {
		if len(os.Args) != 4 {
			fmt.Println("Usage: Link existing newname")
			return
		}

		existing := os.Args[2]
		newname := os.Args[3]

		if len(newname) > 55 {
			fmt.Println("Error: New filename too long (max 55 chars)")
			return
		}

		file, err := os.OpenFile("disk01", os.O_RDWR, 0666)
		if err != nil {
			fmt.Println("Error opening disk:", err)
			return
		}
		defer file.Close()

		sb, err := readSuperblock(file)
		if err != nil {
			fmt.Println("Error reading superblock:", err)
			return
		}

		fnt, _, err := findFNTEntry(file, sb, existing)
		if err != nil {
			fmt.Println("File not found")
			return
		}

		var newEntry FNTEntry
		var fname [56]byte
		copy(fname[:], []byte(newname))
		newEntry.Filename = fname
		newEntry.DABPTIndex = fnt.DABPTIndex

		err = writeFNTEntry(file, sb, newEntry)
		if err != nil {
			fmt.Println("Error writing link:", err)
			return
		}

		fmt.Println("Link created.")
	}

	if command == "Unlink" {
		if len(os.Args) != 3 {
			fmt.Println("Usage: Unlink name")
			return
		}

		filename := os.Args[2]

		file, err := os.OpenFile("disk01", os.O_RDWR, 0666)
		if err != nil {
			fmt.Println("Error opening disk:", err)
			return
		}
		defer file.Close()

		sb, err := readSuperblock(file)
		if err != nil {
			fmt.Println("Error reading superblock:", err)
			return
		}

		fnt, fntOffset, err := findFNTEntry(file, sb, filename)
		if err != nil {
			fmt.Println("File not found")
			return
		}

		dabpt, err := readDABPTEntry(file, sb, fnt.DABPTIndex)
		if err != nil {
			fmt.Println("Error reading DABPT:", err)
			return
		}

		dabpt.RefCount--
		dabptIdx := fnt.DABPTIndex - 1
		dabptBlock := sb.DABPTStart + dabptIdx/4
		dabptOff := (dabptIdx % 4) * 64
		file.Seek(int64(dabptBlock)*BlockSize + int64(dabptOff), 0)
		binary.Write(file, binary.LittleEndian, dabpt)

		if dabpt.RefCount == 0 {
			bpt, err := readBPTEntry(file, sb, dabpt.BPTIndex)
			if err != nil {
				fmt.Println("Error reading BPT:", err)
				return
			}

			for _, block := range bpt.Direct {
				if block != 0 {
					freeBlock(file, block)
				}
			}

			emptyBPT := BPTEntry{}
			bptIdx := dabpt.BPTIndex - 1
			bptBlock := sb.BPTStart + bptIdx/8
			bptOff := (bptIdx % 8) * 32
			file.Seek(int64(bptBlock)*BlockSize + int64(bptOff), 0)
			binary.Write(file, binary.LittleEndian, emptyBPT)

			emptyDABPT := DABPTEntry{}
			file.Seek(int64(dabptBlock)*BlockSize + int64(dabptOff), 0)
			binary.Write(file, binary.LittleEndian, emptyDABPT)
		}

		file.Seek(fntOffset, 0)
		emptyFNT := FNTEntry{}
		binary.Write(file, binary.LittleEndian, emptyFNT)

		fmt.Println("Link removed.")
	}
}