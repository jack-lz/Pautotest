#!/usr/bin/python3
# -*- coding: UTF-8 -*-
import xlrd
import sys

__FirstRow__ = 23
__FirstCol__ = 0
__SheetName__= 'checkList'

__LastAction__ = 'action_final'
__ActionName_action_parallel_a_p__ = 'action_parallel_a_p'
__ActionName_action_a_p__ = 'action_a_p'
__ActionName_action_a__ = 'action_a'
__ActionName_action_n__ = 'action_n'
__ActionName_action_p__ = 'action_p'
__ActionName_action_a_beep__ = 'action_a_beep'
__ActionName_action_waitBeep__ = 'action_waitBeep'
__CaseTotalLines__ = 4
__TableHeadLines__ = 3
__Head_Cols__= 3
__Action_parallel_a_p_Cols__ = 6
__Action_a_p_Cols__ = 5
__Action_a_Cols__ = 4
__Action_a__beep_Cols__ = 6
__Action_n_Cols__ = 2
__Action_final_Cols = 2
__Action_p_Cols__ = 3
__Action_waitBeep_Cols = 3
__TargetXML__ = r'/out.xml'
__ORG_EXECL_NAME__=r'/Template.xlsm'
__TEST_FILE_PATH__=r'/testFile'



def xmlHead():
    return r'<?xml version="1.0" encoding="utf-8"?>' + '\n'
 
def formatAudioFile(audioType, audioPath) :
    return r'<audio_file>' + \
                r'<audio_type>' + audioType + r'</audio_type>' + \
                r'<file_path>' +  audioPath + r'</file_path>' + \
            r'</audio_file>'

def formatParallelAudio(audioType, audioPath, timeout) :
    return r'<audio_file>' + \
                r'<audio_type>' + audioType + r'</audio_type>' + \
                r'<file_path>' +  audioPath + r'</file_path>' + \
                r'<timeout>' +  str(timeout) + r'</timeout>' + \
            r'</audio_file>'

def formatParallelPic(picpath, timeout) :
    return r'<pic_file>' + \
                r'<pic_path>' + picpath + r'</pic_path>' + \
                r'<timeout>' +  str(timeout) + r'</timeout>' + \
            r'</pic_file>'
  
def formatPicPath(picPath) :
    return r'<pic_file>' + picPath + r'</pic_file>'
 
def formatHead(caseNum, caseType, caseName):
    return  r'<case_head>' + \
    			r'<num>'  + str(caseNum) + r'</num>' + \
    			r'<type>' + caseType + r'</type>' + \
    			r'<name>' + str(caseName) + r'</name>' + \
    		r'</case_head>'
 
def formatAction_n(actionType, actionTimer):
    return r'<action>'+ \
                r'<type>'    + actionType  + r'</type>' + \
                r'<timeout>' + str(actionTimer) + r'</timeout>' + \
            r'</action>'
 
def formatAction_waitbeep(actionType, actionTimer, beeptype):
    return r'<action>'+ \
                r'<type beeptype = "' + str(beeptype) + '">'    + actionType  + r'</type>' + \
                r'<timeout>' + str(actionTimer) + r'</timeout>' + \
            r'</action>'

def formatAction_a(actionType, actionTimer, audioType, audioPath) :
    return r'<action>' + \
                r'<type>' + actionType + r'</type>' + \
                formatAudioFile(audioType, audioPath) + \
                r'<timeout>' + str(actionTimer) + r'</timeout>' + \
          r'</action>'

def formatAction_audio_beep(actionType, actionTimer, audioType, audioPath, beeptype) :
    return r'<action>' + \
                r'<type beeptype = "' +  str(beeptype) + r'" >' + actionType + r'</type>' + \
                formatAudioFile(audioType, audioPath) + \
                r'<timeout>' + str(actionTimer) + r'</timeout>' + \
          r'</action>'          

def formateAction_Judge_promote(actionType, actionTimer, audioType, audioPath) :
    return r'<action>' + \
                r'<type>' + actionType + r'</type>' + \
                formatAudioFile(audioType, audioPath) + \
                r'<timeout>' + str(actionTimer) + r'</timeout>' + \
          r'</action>'


def formatAction_beep_beep(actionType, actionTimer, beepStart, beepEnd) :
    return r'<action>' + \
                r'<type beeptypeStart = "' +  str(beepStart) + '" beeptypeEnd = "' + str(beepEnd) + '" >' + actionType + r'</type>' + \
                r'<timeout>' + str(actionTimer) + r'</timeout>' + \
          r'</action>' 

def formatAction_vr_prompt(actionType, actionTimer, audioType, audioPath) :
    return r'<action>' + \
                r'<type>' + actionType + r'</type>' + \
                formatAudioFile(audioType, audioPath) + \
                r'<timeout>' + str(actionTimer) + r'</timeout>' + \
          r'</action>'

def formatAction_beep_audio_beep(actionType, actionTimer, audioType, audioPath, beepStart, beepEnd) :
    return r'<action>' + \
                r'<type beeptypeStart = "' +  str(beepStart) + '" beeptypeEnd = "' + str(beepEnd) + '" >' + actionType + r'</type>' + \
                formatAudioFile(audioType, audioPath) + \
                r'<timeout>' + str(actionTimer) + r'</timeout>' + \
          r'</action>' 

def formatAction_p(actionType, actionTimer, picPath) :
    return r'<action>' + \
                r'<type>' + actionType + r'</type>' + \
                formatPicPath(picPath) + \
                r'<timeout>' + str(actionTimer) + r'</timeout>' + \
          r'</action>' 

 
def formatAction_a_p(actionType, actionTimer, audioType, audioPath, picPath) :
    return r'<action>' + \
                r'<type>' + actionType + r'</type>' + \
                formatAudioFile(audioType, audioPath) + \
                r'<timeout>' + str(actionTimer) + r'</timeout>' + \
                formatPicPath(picPath) + \
          r'</action>'

def formatAction_parallel_a_p(actionType, audioType, audioPath,  autioTimer, picPath, picTimer) :
    return r'<action>' + \
                r'<type>' + actionType + r'</type>' + \
                formatParallelAudio(audioType, audioPath, autioTimer) + \
                formatParallelPic(picPath, picTimer) + \
          r'</action>'
 
def formatAction_final(actionType, actionTimer) :
    return r'<action final="must">' + \
                r'<type>' + actionType + r'</type>' + \
                r'<timeout>' + str(actionTimer) + '</timeout>' + \
            r'</action>'

def formatTestCase(actionListXml) :
	return r'<case>' + actionListXml + r'</case>'

def formatTestXmlFile(caseList) :
	return r'<testcases>' + caseList + r'</testcases>'

def trAction_parallel_a_p(table, curRow, curCol):
	valueRow = curRow + __TableHeadLines__
	typeCol = curCol
	audioFilePathCol = curCol + 1
	audioFileTypeCol = curCol + 2
	audiotFileTimer = curCol + 3
	picFilePathCol = curCol + 4
	picFileTimer = curCol + 5
	typeStr = table.cell(valueRow , typeCol).value
	audioFilePathStr = table.cell(valueRow , audioFilePathCol).value
	audioFileTypeStr = table.cell(valueRow , audioFileTypeCol).value
	autioFileTimerStr = int(table.cell(valueRow , audiotFileTimer).value)
	picFilePathStr = table.cell(valueRow , picFilePathCol).value
	picFileTimerStr = int(table.cell(valueRow , picFileTimer).value)
	return formatAction_parallel_a_p(typeStr, audioFileTypeStr, audioFilePathStr, autioFileTimerStr, picFilePathStr, picFileTimerStr)

def trAction_a_p(table, curRow, curCol):
	valueRow = curRow + __TableHeadLines__
	typeCol = curCol
	timeoutCol = curCol + 1
	audioFilePathCol = curCol + 2
	audioFileTypeCol = curCol + 3
	picFilePathCol = curCol + 4
	typeStr = table.cell(valueRow , typeCol).value
	timeoutStr = int(table.cell(valueRow , timeoutCol).value)
	audioFilePathStr = table.cell(valueRow , audioFilePathCol).value
	audioFileTypeStr = table.cell(valueRow , audioFileTypeCol).value
	picFilePathStr = table.cell(valueRow , picFilePathCol).value
	return formatAction_a_p(typeStr, timeoutStr, audioFileTypeStr, audioFilePathStr, picFilePathStr)

def trAction_p(table, curRow, curCol):
	valueRow = curRow + __TableHeadLines__
	typeCol = curCol
	timeoutCol = curCol + 1
	picFilePathCol = curCol + 2
	typeStr = table.cell(valueRow , typeCol).value
	timeoutStr = int(table.cell(valueRow , timeoutCol).value)
	picFilePathStr = table.cell(valueRow , picFilePathCol).value
	return formatAction_p(typeStr, timeoutStr, picFilePathStr)

def trAction_a(table, curRow, curCol):
	valueRow = curRow + __TableHeadLines__
	typeCol = curCol
	timeoutCol = curCol + 1
	audioFilePathCol = curCol + 2
	audioFileTypeCol = curCol + 3
	typeStr = table.cell(valueRow , typeCol).value
	timeoutStr = int(table.cell(valueRow , timeoutCol).value)
	audioFilePathStr = table.cell(valueRow , audioFilePathCol).value
	audioFileTypeStr = table.cell(valueRow , audioFileTypeCol).value
	return formatAction_a(typeStr, timeoutStr, audioFileTypeStr, audioFilePathStr)


def trAction_a_beep(table, curRow, curCol):
	valueRow = curRow + __TableHeadLines__
	typeCol = curCol
	timeoutCol = curCol + 1
	audioFilePathCol = curCol + 2
	audioFileTypeCol = curCol + 3
	beepStartCol = curCol + 4
	beepEndCol = curCol + 5
	typeStr = table.cell(valueRow , typeCol).value
	timeoutStr = int(table.cell(valueRow , timeoutCol).value)
	audioFilePathStr = table.cell(valueRow , audioFilePathCol).value
	audioFileTypeStr = table.cell(valueRow , audioFileTypeCol).value
	beepStartStr = int(table.cell(valueRow, beepStartCol).value)
	beepEndStr = int(table.cell(valueRow, beepEndCol).value)
	if audioFilePathStr != '-' :
		if beepStartStr != -1 :
			if beepEndStr != -1:
				typeStr = 'judge_vr_beep_prompt_beep'
				return formatAction_beep_audio_beep(typeStr, timeoutStr, audioFileTypeStr, audioFilePathStr, beepStartStr, beepEndStr)
			else :
				typeStr = 'judge_vr_beep_prompt'
				return formatAction_audio_beep(typeStr, timeoutStr, audioFileTypeStr, audioFilePathStr, beepStartStr)
		else :
			if beepEndStr != -1:
				typeStr = 'judge_vr_prompt_beep'
				return formatAction_audio_beep(typeStr, timeoutStr, audioFileTypeStr, audioFilePathStr, beepEndStr)
			else :
				typeStr = 'judge_vr_prompt'
				return formateAction_Judge_promote(typeStr, timeoutStr, audioFileTypeStr, audioFilePathStr)
	else:
		if beepEndStr != -1 and beepStartStr != -1:
			typeStr = 'Judge_vr_beep_beep'
			return formatAction_beep_beep(typeStr, timeoutStr, beepStartStr, beepEndStr)
		else :
			return ""

def trAction_wait_beep(table, curRow, curCol):
	valueRow = curRow + __TableHeadLines__
	typeCol = curCol
	timeoutCol = curCol + 1
	beepTypeCol = curCol + 2
	typeStr = table.cell(valueRow , typeCol).value
	timeoutStr = int(table.cell(valueRow , timeoutCol).value)
	beepTypeStr = int(table.cell(valueRow , beepTypeCol).value)
	return formatAction_waitbeep(typeStr, timeoutStr, beepTypeStr)

def trAction_n(table, curRow, curCol):
	valueRow = curRow + __TableHeadLines__
	typeCol = curCol
	timeoutCol = curCol + 1
	typeStr = table.cell(valueRow , typeCol).value
	timeoutStr = int(table.cell(valueRow , timeoutCol).value)
	return formatAction_n(typeStr, timeoutStr)

def trAction_final(table, curRow, curCol):
	valueRow = curRow + __TableHeadLines__
	timeoutCol = curCol + 1
	typeCol = curCol
	typeStr = table.cell(valueRow , typeCol).value
	timeoutStr = int(table.cell(valueRow , timeoutCol).value)
	return formatAction_final(typeStr, timeoutStr)

def getCaseHead(table, curRow, curCol):
	valueRow = curRow + __TableHeadLines__
	caseNumCol = curCol
	caseTypeCol = curCol + 1
	caseNameCol = curCol + 2
	caseNumVal = int(table.cell(valueRow, caseNumCol).value)
	caseTypeStr = table.cell(valueRow, caseTypeCol).value
	caseNameStr = table.cell(valueRow , caseNameCol).value
	return formatHead(caseNumVal, caseTypeStr, caseNameStr)

def trExecl(worspacePath, sheetName):
	execlFile = worspacePath + __TEST_FILE_PATH__ + __ORG_EXECL_NAME__	
	#open execl
	data = xlrd.open_workbook(execlFile)
	#获取sheet
	table = data.sheet_by_name(sheetName)
	#获取总行数
	totalRows = table.nrows
	totalCols = table.ncols
	if totalCols < __Head_Cols__:
		print("error execl totalCols < __Head_Cols__")
		return False
	currentRow = __FirstRow__	
	caseList = ''
	while currentRow < totalRows :
		currentCol = 0
		actionXml = getCaseHead(table, currentRow, currentCol)
		currentCol += __Head_Cols__
		while currentCol <  totalCols:
			actionName = table.cell(currentRow, currentCol).value
			if actionName == __LastAction__ :
				actionXml += trAction_final(table, currentRow, currentCol)
				currentCol += __Action_final_Cols
				break
			elif actionName == __ActionName_action_a_p__:
				actionXml += trAction_a_p(table, currentRow, currentCol)
				currentCol += __Action_a_p_Cols__
			elif actionName == __ActionName_action_a__ :
				actionXml += trAction_a(table, currentRow, currentCol)
				currentCol += __Action_a_Cols__
			elif actionName == __ActionName_action_n__ :
				actionXml += trAction_n(table, currentRow, currentCol)
				currentCol += __Action_n_Cols__
			elif actionName == __ActionName_action_p__ :
				actionXml += trAction_p(table, currentRow, currentCol)
				currentCol += __Action_p_Cols__
			elif actionName == __ActionName_action_parallel_a_p__ :
				actionXml += trAction_parallel_a_p(table, currentRow, currentCol)
				currentCol += __Action_parallel_a_p_Cols__
			elif actionName == __ActionName_action_a_beep__ :
				actionXml += trAction_a_beep(table, currentRow, currentCol)
				currentCol += __Action_a__beep_Cols__
			elif actionName == __ActionName_action_waitBeep__:
				actionXml += trAction_wait_beep(table, currentRow, currentCol)
				currentCol += __Action_waitBeep_Cols
			else :
				print(actionName, " error ", currentCol)
				break
		currentRow += __CaseTotalLines__
		caseList += formatTestCase(actionXml)
	testXmlFile = xmlHead() + formatTestXmlFile(caseList)
	#print(testXmlFile)
	fo = open(worspacePath + __TEST_FILE_PATH__ + __TargetXML__, 'w+')
	print(worspacePath + __TEST_FILE_PATH__ + __TargetXML__)
	#print ("outputXML is : ", fo.name)
	fo.write(testXmlFile)
	return True
	#cell_value = table.cell(1,1).value



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
