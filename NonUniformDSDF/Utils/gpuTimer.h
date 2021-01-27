#pragma once

/*

	Nanoszekundum pontoss�g� (*) GPU id�z�t�.

	Fontos: nem lehet egym�sba �gyazni a k�l�nb�z� timer-ek k�r�seit sem, am�g
	az OpenGL specifik�ci� nem enged�lyezi a nested glBegin/EndQuery-ket!

	(*): driver �s GPU f�gg�

*/

#include <GL\glew.h>

class gpuTimer
{
public:
	gpuTimer(void);
	~gpuTimer(void);

	void		Start();
	void		Stop();

	double		StopMillis();

	GLuint64	GetLastDeltaNano();
	double		GetLastDeltaMicro();
	double		GetLastDeltaMilli();
private:
	GLuint		m_queries[2];
	int			m_act;
	GLuint64	m_timer_last_delta;
	bool		m_firstUse;
};

