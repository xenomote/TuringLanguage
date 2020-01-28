wget https://ftp.gnu.org/gnu/bison/bison-3.5.1.tar.gz
tar zxvf bison-3.5.1.tar.gz
cd bison-3.5.1/
./configure --prefix=$HOME/usr
make
make install
rm bison-3.5.1.tar.gz bison-3.5.1