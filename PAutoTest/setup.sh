#!/bin/bash
RUN_SCR_FULL_PATH=$0
AUTOTEST_SRC_DIR=$PWD

build_autoTestServer() {
	echo "Install...."
	rm -rf "$AUTOTEST_SRC_DIR/build"
	rm -rf "$AUTOTEST_SRC_DIR/install"
	mkdir "$AUTOTEST_SRC_DIR/build"
	mkdir "$AUTOTEST_SRC_DIR/install"
	echo "start build"
	cd "$AUTOTEST_SRC_DIR/build"
	cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=$AUTOTEST_SRC_DIR/install $AUTOTEST_SRC_DIR/src
	make -j8 || exit 1
	echo "===================================="
	echo "install to $AUTOTEST_SRC_DIR"
	echo "===================================="
	make install
	mv $AUTOTEST_SRC_DIR/install/bin/vats $AUTOTEST_SRC_DIR
	rm -rf "$AUTOTEST_SRC_DIR/build"
	rm -rf "$AUTOTEST_SRC_DIR/install"
}


main() {
	cd ${RUN_SCR_FULL_PATH%/*}
	AUTOTEST_SRC_DIR=$PWD
	echo "work space is $AUTOTEST_SRC_DIR"
	if [ ! -d "$AUTOTEST_SRC_DIR/install" ] && [ ! -d "$AUTOTEST_SRC_DIR/build" ]; then
  		build_autoTestServer
	else
		echo "warning install or build is already in $AUTOTEST_SRC_DIR"
		echo "Install? please [y/n]"
		read answer
		if [ $answer == "y" ] || [ $answer == "Y" ]; then
			build_autoTestServer
		else
			echo "exit"
			exit		
		fi
	fi
}

if [ "$#" -eq 0 ]; then
	main
else
  echo "error input!!"
fi

