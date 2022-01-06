#include <opusfile.h>
#include <sndfile.h>

#include <array>
#include <iostream>

const int kBufferSize = 256;

int main(int argc, char** argv) {
  if (argc != 2) {
    std::cerr << "Usage: ./" << argv[0] << " <infile.opus>" << std::endl;
  }

  int ret = 0;

  OggOpusFile* of = op_open_file(argv[1], &ret);

  if (ret == OP_EFAULT) {
    std::cerr << "Couldn't open " << argv[1] << std::endl;
    exit(1);
  }

  const OpusTags* tags = op_tags(of, -1);
  std::cout << "Vendor:" << tags->vendor << std::endl;
  std::cout << "User Comments:";
  for (int i = 0; i < tags->comments; i++) {
    std::cout << "  " << tags->user_comments[i];
  }
  std::cout << std::endl;

  SF_INFO sfinfo{0};
  sfinfo.samplerate = 48000;
  sfinfo.channels = 2;
  sfinfo.format = SF_FORMAT_WAV | SF_FORMAT_FLOAT;

  SNDFILE* outwav = sf_open("test.wav", SFM_WRITE, &sfinfo);
  if (!outwav) {
    int err = sf_error(outwav);
    std::cerr << "DIDNT OPEN?" << err << "\n";
    exit(1);
  }

  std::array<float, kBufferSize> buffer{0};

  sf_count_t bytes_wrote = 0;
  sf_count_t frames_read = 0;
  while (1) {
    // read from of into buffer;
    frames_read = op_read_float_stereo(of, buffer.begin(), kBufferSize);
    std::cout << "WRITING " << frames_read << std::endl;
    if (frames_read <= 0) {
      std::cerr << "NAE MAIR FRAMES!\n";
      break;
    }
    // write buffer into outwav
    bytes_wrote = sf_writef_float(outwav, buffer.begin(), frames_read);
    std::cout << "WROTE " << bytes_wrote << " to " << outwav << std::endl;
  }

  op_free(of);
  sf_close(outwav);
  return ret;
}
