#!/usr/bin/python3
# -*- coding: UTF-8 -*-
import xlrd
import sys

__FirstRow__ = 0
__FirstCol__ = 0
__SheetName__= 'testConfig'
__AllCaseName__ = r'test_config_all_case'
__TargetXML__ = r'/testConfig.xml'
__ORG_EXECL_NAME__=r'/Template.xlsm'
__TEST_FILE_PATH__=r'/testFile'


def xmlHead():
    return r'<?xml version="1.0" encoding="utf-8"?>' + '\n'

def formatConfigXmlFile(caseList) :
	return r'<case_config>' + caseList + r'</case_config>'
 
def formatTestCase(caseType, caseName) :
	return r'<test_case>' + \
				r'<type>' +     caseType + r'</type>' + \
				r'<case_name>' + caseName + r'</case_name>' + \
			r'</test_case>'

def trEachCase(table, currentRow, currentCol) :
	caseTypeStr = table.cell(currentRow , currentCol).value
	caseNameStr = table.cell(currentRow, currentCol + 1).value
	if '' == caseNameStr:
		if '' != caseTypeStr:
			caseNameStr = __AllCaseName__
		else :
			return ''
	return formatTestCase(caseTypeStr, caseNameStr)

def trExecl(worspacePath, sheetName):
	execlFile = worspacePath + __TEST_FILE_PATH__ + __ORG_EXECL_NAME__	
	#open execl
	data = xlrd.open_workbook(execlFile)
	#获取sheet
	table = data.sheet_by_name(sheetName)
	#获取总行数
	totalRows = table.nrows
	totalCols = table.ncols
	if totalCols <= __FirstRow__:
		print("empty execl")
		return False
	currentRow = __FirstRow__ + 1	
	caseList = ''
	while currentRow < totalRows :
		caseList += trEachCase(table, currentRow, 0)
		currentRow += 1
	testXmlFile = xmlHead() + formatConfigXmlFile(caseList)
	#print(testXmlFile)
	fo = open(worspacePath + __TEST_FILE_PATH__ + __TargetXML__, 'w+')
	print(worspacePath + __TEST_FILE_PATH__ + __TargetXML__)
	fo.write(testXmlFile)
	return True



if __name__=='__main__':
	print(sys.argv)
	if len(sys.argv) >= 2:
		file_path = sys.argv[1]
		# __TargetXML__ = sys.argv[2]
		#print("open file: ", file_path)
		# r'/home/panzejun/pelPro/execlToXml/testFile/test.xlsx'
		trExecl(file_path, __SheetName__)
	else :
		print("please input execl")
