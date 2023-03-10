#pragma once

#define	HASHFS_MAX_LAYER_COUNT			2
#define HASHFS_MAX_LAYER_THLETHHOLD		157
#define	HASH_SEED						7919

class CAprHash
{
public:
	CAprHash(int prSeed = HASH_SEED);
	virtual ~CAprHash();
	CString GetHashDir(LPCTSTR szString);

protected:
	int tranHashValue2NumberInLayer(int hashValue, int layerNumber);
	int getHashValue(LPCTSTR szKeystr);

private:
	int m_prSeed;
	int m_layerCount;
	int m_layerSize;
};

