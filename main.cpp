#include <time.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>

class Boid
{
public:
	Boid()
	{
		m_Position = glm::linearRand(glm::vec2(0.0f, 0.0f), glm::vec2(1280.0f, 640.0f));
		m_Velocity = glm::normalize(glm::linearRand(glm::vec2(-1.0f), glm::vec2(1.0f))) * glm::linearRand(2.0f, 4.0f);
		m_Acceleration = glm::vec2(0.0f, 0.0f);
	}

	void Draw()
	{
		glPushMatrix();
		glTranslatef(m_Position.x, m_Position.y, 0.0f);
		glRotatef(glm::degrees(glm::atan(m_Velocity.y, m_Velocity.x) - 90.0f), 0.0f, 0.0f, 1.0f);
		glScalef(10.0f, 20.0f, 0.0f);
		glBegin(GL_TRIANGLES);
		{
			glVertex2f(-0.5f, -0.5f);
			glVertex2f( 0.0f,  0.5f);
			glVertex2f( 0.5f, -0.5f);
		}
		glEnd();
		glPopMatrix();
	}

	void Update()
	{
		m_Position += m_Velocity;
		m_Velocity += m_Acceleration;
		m_Acceleration *= 0.0f;
		m_Velocity = (glm::normalize(m_Velocity) * m_MaxSpeed);

		if (m_Position.x > 1290.0f)
		{
			m_Position.x = -10.0f;
		}
		else if (m_Position.x < -10.0f)
		{
			m_Position.x = 1290.0f;
		}

		if (m_Position.y > 730.0f)
		{
			m_Position.y = -10.0f;
		}
		else if (m_Position.y < -10.0f)
		{
			m_Position.y = 730.0f;
		}
	}
	
	void Alignment(Boid* p_boids, uint32_t p_boidsCount)
	{
		uint32_t Count = 0;
		glm::vec2 Average(0.0f, 0.0f);

		for (uint32_t i = 0; i < p_boidsCount; i++)
		{
			const float Distance = glm::distance(m_Position, p_boids[i].m_Position);
			if (&p_boids[i] != this && Distance < m_AlignRadius)
			{
				Average += p_boids[i].m_Velocity;
				Count++;
			}
		}
		
		if (Count > 0)
		{
			Average /= static_cast<float>(Count);
			Average = (glm::normalize(Average) * m_MaxSpeed);
			Average -= m_Velocity;
			if (glm::length(Average) > m_AlignForce)
			{
				Average = (glm::normalize(Average) * m_AlignForce);
			}
			m_Acceleration += Average;
		}
	}

	void Cohesion(Boid* p_boids, uint32_t p_boidsCount)
	{
		uint32_t Count = 0;
		glm::vec2 Average(0.0f, 0.0f);

		for (uint32_t i = 0; i < p_boidsCount; i++)
		{
			const float Distance = glm::distance(m_Position, p_boids[i].m_Position);
			if (&p_boids[i] != this && Distance < m_CohesionRadius)
			{
				Average += p_boids[i].m_Position;
				Count++;
			}
		}

		if (Count > 0)
		{
			Average /= static_cast<float>(Count);
			Average -= m_Position;
			Average = (glm::normalize(Average) * m_MaxSpeed);
			Average -= m_Velocity;
			if (glm::length(Average) > m_CohesionForce)
			{
				Average = (glm::normalize(Average) * m_CohesionForce);
			}
			m_Acceleration += Average;
		}
	}

	void Separation(Boid* p_boids, uint32_t p_boidsCount)
	{
		uint32_t Count = 0;
		glm::vec2 Average(0.0f, 0.0f);

		for (uint32_t i = 0; i < p_boidsCount; i++)
		{
			const float Distance = glm::distance(m_Position, p_boids[i].m_Position);
			if (&p_boids[i] != this && Distance < m_SeparationRadius)
			{
				glm::vec2 Difference = (m_Position - p_boids[i].m_Position);
				Difference *= (1.0f / Distance);
				Average += Difference;
				Count++;
			}
		}

		if (Count > 0)
		{
			Average /= static_cast<float>(Count);
			Average = (glm::normalize(Average) * m_MaxSpeed);
			Average -= m_Velocity;
			if (glm::length(Average) > m_SeparationForce)
			{
				Average = (glm::normalize(Average) * m_SeparationForce);
			}
			m_Acceleration += Average;
		}
	}

private:
	glm::vec2 m_Position;
	glm::vec2 m_Velocity;
	glm::vec2 m_Acceleration;

	const float m_AlignForce = 0.2f;
	const float m_CohesionForce = 0.2f;
	const float m_SeparationForce = 0.5f;

	const float m_AlignRadius = 100.0f;
	const float m_CohesionRadius = 100.0f;
	const float m_SeparationRadius = 30.0f;

	const float m_MaxSpeed = 4.0f;
};

int main(int argc, char** argv)
{
	srand(time(nullptr));

	glfwInit();
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	GLFWwindow* Window = glfwCreateWindow(1280, 720, "Boids", nullptr, nullptr);
	glfwMakeContextCurrent(Window);
	glfwSwapInterval(1);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(1.0f, 1.0f, 1.0f, 0.75f);
	glMatrixMode(GL_PROJECTION);
	glOrtho(0.0, 1280.0, 0.0, 720.0, 0.0, 1.0);
	glMatrixMode(GL_MODELVIEW);

	const uint32_t BoidCount = 250;
	Boid* Boids = new Boid[BoidCount];

	while (!glfwWindowShouldClose(Window))
	{
		glfwPollEvents();
		glClear(GL_COLOR_BUFFER_BIT);

		for (uint32_t i = 0; i < BoidCount; i++)
		{
			Boids[i].Alignment(Boids, BoidCount);
			Boids[i].Cohesion(Boids, BoidCount);
			Boids[i].Separation(Boids, BoidCount);
			Boids[i].Update();
			Boids[i].Draw();
		}

		glfwSwapBuffers(Window);
	}

	delete[] Boids;
	glfwTerminate();
	return 0;
}