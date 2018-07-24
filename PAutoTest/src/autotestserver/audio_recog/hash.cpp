#include "hash.h"
#include "LOG.h"


//外部函数
inline size_t HashTableOffset(size_t f1, size_t f2_f1, size_t t){
	return (f1 * (1 << 12) + f2_f1 * (1 << 6) + t);
}

//类方法
/************************************  Functions for build tracks.(iFlyBuild) ************************************************/
//此函数用于对于iFlyBuild的情况，从wav到Hash_Table.				//Finished.
void THash::BuildInit(){
	//Initialize memory value.
	pValueStart = (size_t*)malloc(sizeof(size_t) * HashKeyNum * ValuePerBlock * BlockNum);
	if (pValueStart == NULL)
        LOG("pValueStart is NULL! \n");
	memset(pValueStart, 0, sizeof(size_t) * HashKeyNum * ValuePerBlock * BlockNum);
	//连续内存中，初始化已分配的结尾地址。
//	pValueEnd = pValueStart + sizeof(size_t) * ValuePerBlock;
    pValueEnd = pValueStart;
	//初始化HashKey表
	key_info = (HashKeyInfo*)malloc(sizeof(HashKeyInfo) * HashKeyNum);
	for (int i = 0; i < HashKeyNum; i++){
		key_info[i].next = NULL;
		key_info[i].length = 0;
	}
}

void THash::BuildUnInit(){
	for (int i = 0; i<HashKeyNum; i++){
		HashKeyInfo *p = key_info[i].next;
		HashKeyInfo *q;
		while (p){
			q = p;
			p = p->next;
			free(q);
		}
	}
	free(key_info);
	free(pValueStart);
}

//加歌名，更新歌曲数。
void THash::AddSongList(const char *filename){
    strncpy(song_list[song_num],filename, strlen(filename) + 1);
	song_num++;
}

//往Value的内存块里加数据，更新Key_table.
void THash::InsertHash(size_t f1, size_t f2_f1, size_t t, size_t id, size_t offset){
	//异常处理
	if (max_offset < offset) {
		max_offset = offset;
	}
	if (id >(1 << ID_BITS) - 1 || offset > (1 << OFFSET_BITS) - 1){
        //LOG("The id/time offset overflow. id:%d  (1 << ID_BITS) - 1:%d offset:%d (1 << OFFSET_BITS) - 1:%d  \n", id, (1 << ID_BITS) - 1, offset, (1 << OFFSET_BITS) - 1);
		return;
	}
	//当前key的地址
	HashKeyInfo *pKey = &key_info[HashTableOffset(f1, f2_f1, t)];
	if (pKey->length%ValuePerBlock==0){
		HashKeyInfo * pNode = (HashKeyInfo*)malloc(sizeof(HashKeyInfo));
		/* 内存溢出 */
		if ((pValueEnd - pValueStart) > OverFlowThreshold){
            //LOG("Memory out.\n");
			return;
		}
		pNode->start = pValueEnd;
//		pValueEnd += ValuePerBlock*sizeof(size_t);
        pValueEnd += ValuePerBlock;
		pNode->length = 0;
		pNode->next = pKey->next;
		pKey->next = pNode;
	}
	size_t* pValue = pKey->next->start + pKey->next->length;
	*pValue = (size_t)((id << OFFSET_BITS) + offset);
	pKey->next->length++;
	pKey->length++;
	/* For File Write */
	data_num++;

	if (max_keylen < pKey->length) {
		max_keylen = pKey->length;
	}
}

//将Hash表往文件里刷（不是刷整个内存，这样会在iFlySelect里浪费内存空间）
void THash::Hash2File(const char* filename){
    FILE *fp = fopen(filename, "wb");
	//Write SongName
	fwrite(&song_num, sizeof(size_t), 1, fp);
    //LOG("共%zu首歌\n", song_num);
	for (size_t i = 0; i<song_num; i++)
		fwrite(song_list[i], sizeof(char), strlen(song_list[i]) + 1, fp);
	//Write hash table.
	fwrite(&data_num, sizeof(size_t), 1, fp);
	for (int i = 0; i<HashKeyNum; i++){
		HashKeyInfo *p = key_info[i].next;
		while (p){
			fwrite(p->start, sizeof(size_t), p->length, fp);
			p = p->next;
		}
	}
	size_t start_place = 0;
	for (int i = 0; i<HashKeyNum; i++){
		fwrite(&start_place, sizeof(size_t), 1, fp);
		fwrite(&key_info[i].length, sizeof(size_t), 1, fp);
		start_place += key_info[i].length;
	}
	fclose(fp);
}

/************************************  Functions for select tracks.(iFlySelect) ************************************************/

void THash::File2Hash(const char *filename){
    FILE *fp = fopen(filename, "wb");
	char *chp;
	fread(&song_num, sizeof(size_t), 1, fp);
    //LOG("------------\n");
    //LOG("数据库中共有 %zu 首歌的特征数据.\n", song_num);
	for (size_t i = 0; i<song_num; i++){
		chp = song_list[i];
		do{
			fread(chp, sizeof(char), 1, fp);
		} while (*(chp++) != 0);
	}
	fread(&data_num, sizeof(size_t), 1, fp);
	pValueStart = (size_t*)malloc(sizeof(size_t)* data_num);
	fread(pValueStart, sizeof(size_t), data_num, fp);
	key_table = (HashKeyTable*)malloc(sizeof(HashKeyTable)*HashKeyNum);
	fread(key_table, sizeof(size_t) * 2, HashKeyNum, fp);
	for (int i = 0; i<HashKeyNum; i++)
		key_table[i].start = (size_t)key_table[i].start + pValueStart;
}

void THash::ReBuildInit(){
	vote_table = (short **)malloc(sizeof(short*)* song_num);
	for (size_t i = 0; i<song_num; i++){
		vote_table[i] = (short *)malloc(sizeof(short)* (1 << OFFSET_BITS));
		assert(vote_table[i] != NULL);
	}
}

void THash::VoteInit(){
	for (size_t i = 0; i<song_num; i++){
		memset(vote_table[i], 0, sizeof(short)* (1 << OFFSET_BITS));
	}
	return;
}

void THash::Vote(size_t f1, size_t f2_f1, size_t t, size_t offset){
	HashKeyInfo *pKey = &key_info[HashTableOffset(f1, f2_f1, t)];
	size_t length = pKey->length;
	while (length){
		length--;
		/* size_t offset_value = (*(pKey->start + length) << ID_BITS) >> ID_BITS; */
                // LOG("__FILE__:%s __FUNCTION__:%s __LINE__:%d length:%d \n ", __FILE__, __FUNCTION__, __LINE__, length);
					// size_t offset_value = (*(pKey->start + length)) & 0x00003FFF ; // 0x00003FFF means the first 14 bits in total 32 bits.
			
                // LOG("__FILE__:%s __FUNCTION__:%s __LINE__:%d \n ", __FILE__, __FUNCTION__, __LINE__);
                    /* LOG("Here5,%zu, %zu\n",*(pKey->start + length), offset_value);  */

		size_t offset_value = (*(pKey->next->start + length)) & 0x0000FFFF; // 0x00003FFF means the first 14 bits in total 32 bits.
		if (offset_value < offset)
			continue;
		vote_table[(*(pKey->next->start + length)) >> OFFSET_BITS][offset_value - offset]++;
		++vote_times;
	}
	return;
}

size_t THash::VoteResult(size_t &offset){
	size_t result = 0;
	short max = -1;
	for (size_t i = 0; i < song_num; i++){
		for (size_t j = 0; j < (1 << OFFSET_BITS); j++)
			if (vote_table[i][j] > max){
				max = vote_table[i][j];
				result = i;
				offset = j;
			}
	}
	return max;
}

THash::THash(){
	pValueStart = NULL;
	pValueEnd = NULL;
	vote_table = NULL;
	data_num = 0;
	song_num = 0;
	key_info = NULL;
	max_keylen = 0;
	max_offset = 0;
	vote_times = 0;
	song_list = (char **)malloc(MAX_SONG_NUM*sizeof(char*));
	for (int i = 0; i<MAX_SONG_NUM; i++)
		song_list[i] = (char *)malloc(MAX_SONG_LEN*sizeof(char));
}

THash::~THash(){
	for (int i = 0; i<MAX_SONG_NUM; i++)
		free(song_list[i]);
	free(song_list);
	 for (size_t j = 0; j < song_num; j++)
		free(vote_table[j]);
	free(vote_table); 

}
