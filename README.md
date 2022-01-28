# OggOpusDec0der
clang++ -o ofile -I /Users/sideboard/homebrew/include/opus -I/Users/sideboard/homebrew/include/ -I /Users/sideboard/homebrew/Cellar/libogg/1.3.5/include/ -L /Users/sideboard/homebrew/lib/ -lopusfile -lsndfile ofile.cpp

clang++ -o oofd -I /Users/sideboard/homebrew/include/opus -I/Users/sideboard/homebrew/include/ -I /Users/sideboard/homebrew/Cellar/libogg/1.3.5/include/ -L /Users/sideboard/homebrew/lib/ -lopus -lsndfile oofd.cpp

clang++ -o oofd  -I/Users/sideboard/homebrew/include/ -I /Users/sideboard/Code/OPUSCOMPILE/include -I /Users/sideboard/homebrew/Cellar/libogg/1.3.5/include/ -L  /Users/sideboard/Code/OPUSCOMPILE/lib -L /Users/sideboard/homebrew/lib/ -lopus -lsndfile oofd.cpp
