#include <portaudio.h>

#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <vector>

// implementation based upon
// https://datatracker.ietf.org/doc/html/rfc7845
// An Ogg Opus stream is organized as follows:
//         Page 0         Pages 1 ... n        Pages (n+1) ...
//   +------------+ +---+ +---+ ... +---+ +-----------+ +---------+ +--
//   |            | |   | |   |     |   | |           | |         | |
//   |+----------+| |+-----------------+| |+-------------------+ +-----
//   |||ID Header|| ||  Comment Header || ||Audio Data Packet 1| | ...
//   |+----------+| |+-----------------+| |+-------------------+ +-----
//   |            | |   | |   |     |   | |           | |         | |
//   +------------+ +---+ +---+ ... +---+ +-----------+ +---------+ +--
//   ^      ^                           ^
//   |      |                           |
//   |      |                           Mandatory Page Break
//   |      |
//   |      ID header is contained on a single page
//   |
//   'Beginning Of Stream'

const std::string testfile("gaborNote.opus");

// Ogg Header Magic Number
const std::string kOggCapturePattern{"OggS"};

// Opus Header IDs
const std::string kOpusIdHeader{"OpusHead"};
const std::string kOpusCommentHeader{"OpusTags"};

// variable header size, based on number of segments.
const int kMinHeaderSize = 27;
const int kPageSegmentField = kMinHeaderSize - 1;

// searches for pattern string in the buffer reference starting at buffer_idx.
// Used for looking for various Magic Ids in Ogg bistream without
// copying values.
bool MatchesPattern(const std::string& pattern,
                    const std::vector<unsigned char>& buffer, int buffer_idx) {
  int pattern_len = pattern.length();
  if (buffer.size() < buffer_idx || buffer.size() < pattern_len) return false;
  if (buffer_idx + pattern_len > buffer.size()) return false;

  for (int i = 0, idx = buffer_idx; i < pattern_len; i++, idx++)
    if (pattern[i] != buffer[idx]) return false;
  return true;
}

void DecodeOggPage(int page_num, std::vector<unsigned char> page) {
  // https://datatracker.ietf.org/doc/html/rfc7845
  // page #0 should only contain ID Header
  // page #1..n should only contain comments
  // page #(n+1)... onwards should be Audio Data Packets
}

void DecodeOggStream(std::vector<unsigned char>& buffer) {
  int idx = 0;
  int current_page = 0;

  bool have_header_page = false;
  bool have_comment_page = false;

  while (idx < buffer.size()) {
    int bytes_remaining = buffer.size() - idx;

    if (bytes_remaining > kMinHeaderSize) {
      // STEP 1 - identify page
      if (MatchesPattern(kOggCapturePattern, buffer, idx)) {
        std::cout << "\n\nPAGE:" << current_page << std::endl;

        std::stringstream ss_bs;
        ss_bs << std::hex << static_cast<unsigned int>(buffer[idx + 17])
              << std::hex << static_cast<unsigned int>(buffer[idx + 16])
              << std::hex << static_cast<unsigned int>(buffer[idx + 15])
              << std::hex << static_cast<unsigned int>(buffer[idx + 14]);
        std::string bitstream_serial = ss_bs.str();

        std::cout << "BITSTREAM SERIAl:" << bitstream_serial << std::endl;

        std::stringstream ss_sq;
        ss_sq << std::hex << static_cast<unsigned int>(buffer[idx + 21])
              << std::hex << static_cast<unsigned int>(buffer[idx + 20])
              << std::hex << static_cast<unsigned int>(buffer[idx + 19])
              << std::hex << static_cast<unsigned int>(buffer[idx + 18]);
        std::string page_sequence_number = ss_sq.str();
        std::cout << "PAGESEQ:" << page_sequence_number << std::endl;

        unsigned int num_page_segments = buffer[idx + kPageSegmentField];
        std::cout << "Num segments:" << num_page_segments << std::endl;

        auto header_type = static_cast<unsigned int>(buffer[idx + 5]);
        bool is_first_page_of_bitstream = header_type & 2;
        bool is_last_page_of_bitstream = header_type & 4;

        int data_start_idx = idx + kMinHeaderSize + num_page_segments;

        int total_data_size_bytes = 0;
        for (int i = 0; i < num_page_segments; i++) {
          int lacing_val =
              static_cast<unsigned int>(buffer[idx + kMinHeaderSize + i]);
          total_data_size_bytes += lacing_val;
        }

        int header_size_bytes = kMinHeaderSize + num_page_segments;
        std::cout << "HEADER SIZE IS " << header_size_bytes << std::endl;
        std::cout << "DATA START IDX:" << data_start_idx << "\n";
        std::cout << "DATA SIZE IS " << total_data_size_bytes << std::endl;

        int total_page_size = header_size_bytes + total_data_size_bytes;
        std::cout << "PAGE #:" << current_page << " is " << total_page_size
                  << " bytes." << std::endl;

        if (MatchesPattern(kOpusIdHeader, buffer, data_start_idx)) {
          std::cout << "WOOOOP OP!\n";
        }
        std::cout << "fistpage:?" << is_first_page_of_bitstream
                  << " current page:" << current_page << std::endl;
        if (is_first_page_of_bitstream && current_page == 0 &&
            MatchesPattern(kOpusIdHeader, buffer, data_start_idx)) {
          std::cout << "**HEADER IDX!\n";
          have_header_page = true;
        } else if (have_header_page &&
                   MatchesPattern(kOpusCommentHeader, buffer, data_start_idx)) {
          std::cout << "**COMMENT IDX!\n";
          have_comment_page = true;
        } else {  // data pages
          std::cout << "**DATA PAGES!\n";
        }

        idx += total_page_size;
        std::cout << "NEXT INDEX EXPECTED AT " << idx << std::endl;

        current_page++;
      } else {
        idx++;
      }
    }
  }
  std::cout << "\nDone! IDX is at:" << idx << std::endl;
}

int main(int argc, char** argv) {
  std::string infile = testfile;

  if (argc > 1) {
    infile = argv[1];
  }

  std::cout << "Decoding " << infile << std::endl;

  std::ifstream input(infile, std::ios::binary);
  std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(input), {});
  std::cout << "NEW BUFFER SIZE:" << buffer.size() << std::endl;
  DecodeOggStream(buffer);
}
