#pragma once

// Класс для автоматического управления флагом
class Flag
{
public:
	Flag(bool* p, bool v)
	{
		this->p = p;
		this->v = *p;
		*p = v;
	}
	~Flag()
	{
		*p = v;
	}
	void set() 
	{
		*p = true;
	}
	void reset()
	{
		*p = false;
	}
	void clear()
	{
		*p = v;
	}
private:
	bool* p;
	bool v;
};