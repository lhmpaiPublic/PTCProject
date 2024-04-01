#pragma once

class CPlcIoBase
{
public :
	CPlcIoBase();
	virtual ~CPlcIoBase();

private:
	virtual int OpenPlcIo(void) = 0;
	virtual int ClosePlcIo(void) = 0;

};

