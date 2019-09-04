// CPSC457 Fall 2017, University of Calgary
// Skeleton C++ program for Q7 of Assignment 5.
//
// The program reads in the input, then calls the (wrongly implemented) checkConsistency()
// function, and finally formats the output.
//
// You only need to reimplement the checkConsistency() function.
//
// Author: Pavol Federl (pfederl@ucalgary.ca or federl@gmail.com)
// Date: November 29, 2017
// Version: 1

#include <stdio.h>
#include <string>
#include <vector>

typedef std::string SS;
typedef std::vector<SS> VS;


struct DEntry {
    SS fname = SS( 4096, 0);
    int size = 0;
    int ind = 0;
    bool tooManyBlocks = false;
    bool tooFewBlocks = false;
    bool hasCycle = false;
    bool sharesBlocks = false;
};

static SS join( const VS & toks, const SS & sep) {
    SS res;
    bool first = true;
    for( auto & t : toks) { res += (first ? "" : sep) + t; first = false;}
    return res;
}

// re-implement this function
//
// Parameters:
//   blockSize - contains block size as read in from input
//   files - array containing the entries as read in from input
//   fat - array representing the FAT as read in from input
// Return value:
//   the function should return the number of free blocks
//   also, for ever entry in the files[] array, you need to set the appropriate flags:
//      i.e. tooManyBlocks, tooFewBlocks, hasCycle and sharesBlocks

int contains(int integer, std::vector<int> & vec, int too) {
	for (int i = 0; i < too; i++) {
		if (vec[i] == integer) {
			return 1;
		}
	}
	return 0;
}

int checkConsistency( int blockSize, std::vector<DEntry> & files, std::vector<int> & fat)
{
	/*
    // make the first entry contain no errors
    if( files.size() > 0) {
        files[0].hasCycle = false;
        files[0].tooFewBlocks = false;
        files[0].tooManyBlocks = false;
        files[0].sharesBlocks = false;
    }

    // make the 2nd entry contain one error
    if( files.size() > 1) {
        files[1].hasCycle = true;
        files[1].tooFewBlocks = false;
        files[1].tooManyBlocks = false;
        files[1].sharesBlocks = false;
    }

    // make the 3rd entry contain two errors
    if( files.size() > 2) {
        files[2].hasCycle = false;
        files[2].tooFewBlocks = false;
        files[2].tooManyBlocks = true;
        files[2].sharesBlocks = true;
    }
	*/
	std::vector<int> fat_copy;
	fat_copy = fat;
	int used_blocks = 0;
	for (int i = 0; i < files.size(); i++) {
		//printf("checking file %d\n", i);
		int not_done = 1;
		int count = 0;
		int next_index = files[i].ind;
		int next_true_index = next_index;
		int previous_index;
		std::vector<int> files_blocks(fat.size());
		int end = 0;
		while (not_done) {
			//printf("next is %d\n and actual next is %d\n", next_index, next_true_index);
			// if next index is -1 then we have reached the end
			if (next_index == -1) {
				not_done = 0;
				//printf("got to end\n");
			}
			// if next index is greater than -1 we know that we have not reached the end, and since next index is not contained n the files blocks we have not reached a cycle
			else if (next_index > -1 && contains(next_index, files_blocks, count) == 0) {
				// add this index to the collection of blocks the file has used
				files_blocks[count] = next_true_index;
				count++;
				
				// update previous index and next index values
				previous_index = next_index;
				next_index = fat_copy[previous_index];
				next_true_index = fat[previous_index];

				// if next index is greater than or equal to -1 then we know the block we just read has been unused and to increment used blocks
				if (next_index >= -1) used_blocks++;

				// update the entry in our copy of fat to indicate that it has been used by us
				fat_copy[previous_index] = 0 - 2 - i;
				//printf("number is valid\n");
			}
			// if next index is -1, fat copy has been modified here by something so the previous block has been used before
			else if (next_index < -1 || contains(next_index, files_blocks, count) == 1) {
				//printf("number is either cycle or shared\n");
				// if next index = 0 - 2 - i then it was modified by the same file and we are in a cycle
				if (next_index == 0 - 2 - i || contains(next_index, files_blocks, count) == 1) {
					not_done = 0;
					files[i].hasCycle = true;
				}
				// if not than it was modified by another file, set that files shared blocks value to true and also this ones and continue with the next true index specified in fat
				else if (next_index != 0 - 2 - i) {
					// change the shares blocks value of the other file to true
					int shared_elem_index = (next_index + 2) * (-1);
					files[shared_elem_index].sharesBlocks = true;

					if (next_true_index == -1){
						files[i].sharesBlocks = true;
						not_done = 0;
					}
					else {
						// add the true next index to our blocks
						files_blocks[count] = next_true_index;
						// set out shares blocks value to true
						files[i].sharesBlocks = true;
						// update next index
						next_index = fat_copy[next_true_index];
						next_true_index = fat[next_true_index];
						count++;
					}
				}
			}
		}
		
		int allocated_space = count * blockSize;
		//printf("files size is %d, allocated space is %d, math is %d\n", files[i].size, allocated_space, allocated_space - blockSize);
		if (allocated_space < files[i].size) {
			files[i].tooFewBlocks = true;
		}
		if ((allocated_space - blockSize) > files[i].size) {
			
			files[i].tooManyBlocks = true;
		}
	}

    // finally, return the number of free blocks
    return fat.size() - used_blocks;
    
}



int main()
{
    try {
        // read in blockSize, nFiles, fatSize
        int blockSize, nFiles, fatSize;
        if( 3 != scanf( "%d %d %d", & blockSize, & nFiles, & fatSize))
            throw "cannot read blockSize, nFiles and fatSize";
        if( blockSize < 1 || blockSize > 1024) throw "bad block size";
        if( nFiles < 0 || nFiles > 50) throw "bad number of files";
        if( fatSize < 1 || fatSize > 200000) throw "bad FAT size";
        // read in the entries
        std::vector<DEntry> entries;
        for( int i = 0 ; i < nFiles ; i ++ ) {
            DEntry e;
            if( 3 != scanf( "%s %d %d", (char *) e.fname.c_str(), & e.ind, & e.size))
                throw "bad file entry";
            e.fname = e.fname.c_str();
            if( e.fname.size() < 1 || e.fname.size() > 16)
                throw "bad filename in file entry";
            if( e.ind < -1 || e.ind >= fatSize) throw "bad first block in fille entry";
            if( e.size < 0 || e.size > 1073741824) throw "bad file size in file entry";
            entries.push_back( e);
        }
        // read in the FAT
        std::vector<int> fat( fatSize);
        for( int i = 0 ; i < fatSize ; i ++ ) {
            if( 1 != scanf( "%d", & fat[i])) throw "could not read FAT entry";
            if( fat[i] < -1 || fat[i] >= fatSize) throw "bad FAT entry";
        }

        int nFreeBlocks = checkConsistency( blockSize, entries, fat);
        size_t maxflen = 0;
        for( auto & e : entries ) maxflen = std::max( maxflen, e.fname.size());
        SS fmt = "  %" + std::to_string( maxflen) + "s: %s\n";

        printf( "Issues with files:\n");
        for( auto & e : entries ) {
            VS issues;
            if( e.tooFewBlocks) issues.push_back( "not enough blocks");
            if( e.tooManyBlocks) issues.push_back( "too many blocks");
            if( e.hasCycle) issues.push_back( "contains cycle");
            if( e.sharesBlocks) issues.push_back( "shares blocks");
            printf( fmt.c_str(), e.fname.c_str(), join( issues, ", ").c_str());
        }
        printf( "Number of free blocks: %d\n", nFreeBlocks);

    }
    catch( const char * err) {
        fprintf( stderr, "Error: %s\n", err);
    }
    catch( ... ) {
        fprintf( stderr, "Errro: unknown.\n");
    }
    return 0;
}
