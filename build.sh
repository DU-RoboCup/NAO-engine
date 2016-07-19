 #!/bin/bash
mkdir -p /home/vagrant/NAO/NAO-engine/build
cd /home/vagrant/NAO/NAO-engine/build
cmake ../ -DUSE_CROSS=NO
make
rm -rf *
cmake ../ -DUSE_CROSS=YES
make
cd ..
