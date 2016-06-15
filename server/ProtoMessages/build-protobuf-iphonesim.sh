XCODEDIR=`xcode-select --print-path`
IOS_SDK_VERSION=`xcrun --sdk iphoneos --show-sdk-version`
MIN_SDK_VERSION=6.0

DARWIN=darwin`uname -r`

IPHONESIMULATOR_PLATFORM=`xcrun --sdk iphonesimulator --show-sdk-platform-path`
IPHONESIMULATOR_SYSROOT=`xcrun --sdk iphonesimulator --show-sdk-path`

CC=clang
CXX=clang

CFLAGS="${CLANG_VERBOSE} -DNDEBUG -g -O0 -pipe -fPIC -fcxx-exceptions"
CXXFLAGS="${CLANG_VERBOSE} ${CFLAGS} -std=c++11 -stdlib=libc++"

LDFLAGS="-stdlib=libc++"
LIBS="-lc++ -lc++abi"

# make distclean
# ./configure --build=x86_64-apple-${DARWIN} --host=x86_64-apple-${DARWIN} --disable-shared --exec-prefix=/usr/local/lib/platform/x86_64-sim "CC=${CC}" "CFLAGS=${CFLAGS} -miphoneos-version-min=${MIN_SDK_VERSION} -arch x86_64 -isysroot ${IPHONESIMULATOR_SYSROOT}" "CXX=${CXX}" "CXXFLAGS=${CXXFLAGS} -arch x86_64 -isysroot ${IPHONESIMULATOR_SYSROOT}" LDFLAGS="-arch x86_64 -miphoneos-version-min=${MIN_SDK_VERSION} ${LDFLAGS}" "LIBS=${LIBS}"
# make
# make install

make distclean
./configure --build=x86_64-apple-${DARWIN} --host=i386-apple-${DARWIN} --disable-shared --exec-prefix=/usr/local/platform/i386-sim "CC=${CC}" "CFLAGS=${CFLAGS} -miphoneos-version-min=${MIN_SDK_VERSION} -arch i386 -isysroot ${IPHONESIMULATOR_SYSROOT}" "CXX=${CXX}" "CXXFLAGS=${CXXFLAGS} -arch i386 -isysroot ${IPHONESIMULATOR_SYSROOT}" LDFLAGS="-arch i386 -miphoneos-version-min=${MIN_SDK_VERSION} ${LDFLAGS}" "LIBS=${LIBS}"
make
make install