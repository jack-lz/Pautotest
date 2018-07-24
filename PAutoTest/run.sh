#!/bin/bash
PRO_SRC_PATH=$PWD
WORK_SPACE_PATH=$PWD

run_testCase() {
	$1/vats $1 $WORK_SPACE_PATH/report.xml  $WORK_SPACE_PATH/out  $1/testFile/config17.txt	
	echo $1/testFile/Template.xlsm
	python $1/script/parseReport/ReportToExecl.py $WORK_SPACE_PATH/report.xml $1/testFile/Template.xlsm
}

if [ "$#" -gt 0 ]; then
	if [ ! -f $1 ]; then
		echo "please input Test case path"
		exit 1
	fi
	RUN_SCR_FULL_PATH=$0
	PRO_SRC_PATH=${RUN_SCR_FULL_PATH%/*}
	cp $1 $PRO_SRC_PATH/testFile/Template.xlsm || exit 1
	cd $PRO_SRC_PATH
	run_testCase $PWD $currentDir
	cd $WORK_SPACE_PATH
	cp $PRO_SRC_PATH/testFile/Template.xlsm $1
else
	run_testCase $PWD $PWD/testFile/Template.xlsm
fi

