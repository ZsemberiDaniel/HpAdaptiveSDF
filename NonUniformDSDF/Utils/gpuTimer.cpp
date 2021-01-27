#include "gpuTimer.h"


gpuTimer::gpuTimer(void)
{
	glGenQueries(2, m_queries);

	m_act = 0;
	m_firstUse = true;
}


gpuTimer::~gpuTimer(void)
{
	glDeleteQueries(2, m_queries);
}

void gpuTimer::Start()
{
	if (!m_firstUse)
		glGetQueryObjectui64v(m_queries[m_act], GL_QUERY_RESULT, &m_timer_last_delta);
	else
		m_firstUse = false;

	m_act = 1 - m_act;

	glBeginQuery(GL_TIME_ELAPSED, m_queries[m_act]);
}

void gpuTimer::Stop()
{
	glEndQuery(GL_TIME_ELAPSED);
}

double gpuTimer::StopMillis()
{
	glEndQuery(GL_TIME_ELAPSED);
	return m_timer_last_delta/1000000.0f;
}

GLuint64 gpuTimer::GetLastDeltaNano()
{
	return m_timer_last_delta;
}

double gpuTimer::GetLastDeltaMicro()
{
	return m_timer_last_delta/1000.0f;
}

double gpuTimer::GetLastDeltaMilli()
{
	return m_timer_last_delta/1000000.0f;
}