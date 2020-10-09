#include "Angel.h"
#include "mesh.h"
#include "FreeImage.h"
#include "math.h"
#include "time.h"
#include "Mesh_Painter.h"
#include<iostream>
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "FreeImage.lib")

typedef Angel::vec4 point4;
typedef Angel::vec4 color4;

int mouse_x = 512, mouse_y = 512;
// 相机参数
float radius = 2.5	;
float rotateAngle = 0.0;
float upAngle = 20.0;
vec4 at = vec4(0, 0, 0, 1);
vec4 eye = vec4(0, 0, 0, 1);
vec4 direction;
vec3 boomDirection;
int choice = 2;
int score = 0;
float V=0.12; //直升机速度m/dt
int dt = 40; //ms
float g = 9.8;
int t = 0;
float high = 0;
float disX = 0;
float disZ = 0;
mat4 saveTransform;

vec4 KBeyeMove = vec4(0, 0, 0, 0);
bool isDrop = false;
bool isSpr = false;
int sprTime = 0;
const int ptcNum=500;

float degree = 0;

My_Mesh** screw;
My_Mesh* Helicopter;
My_Mesh* boom;
My_Mesh* rosette;
My_Mesh** ptc;
//光照
vec3 lightPos = vec3(100, 100, 100);
std::vector<My_Mesh*> my_meshs;
Mesh_Painter* mp_;

void init()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glClearColor(0.9, 0.9, 0.9, 1.0);
}

void
display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	float x2, y2, z2, x1, y1, z1,x3,y3,z3;
	Helicopter->get_translate(x2, y2, z2);
	at = vec4(x2, y2, z2, 0);
	//at = vec4(0, 0, 0, 0);
	vec4 up = vec4(0, 1, 0, 0);
	Helicopter->get_theta(x3, y3, z3);

	//eye = vec4(x1 + sin( (y2 + 10) * DegreesToRadians), y1, z1 - cos((y2+10) * DegreesToRadians));

	float x = radius * cos(x3 * M_PI / 180.0) * sin(y3 * M_PI / 180.0) + x2;
	float y = radius * sin(x3 * M_PI / 180.0) + y2;
	float z = radius * cos(x3 * M_PI / 180.0) * cos(y3 * M_PI / 180.0) + z2;
	
	direction = normalize(at - vec4(x, y, z, 1));
	eye = at + direction + KBeyeMove;
	mat4 viewMatrix = LookAt(eye, at, up);
	
	for (int i = 0; i < 2; i++)
	{
		screw[i]->set_translate(x2, y2, z2);
		screw[i]->Opoints = vec3(0, 0.35, 0.31);
	}
	
	//screw[i]->set_theta(x3, y3, z3);

	if (!isDrop)
	{
		boom->set_translate(x2, y2, z2);
		boom->Opoints = vec3(0, 0, 0.31);
		boom->set_theta(x3, y3, z3);
		boom->set_theta_step(0, 1, 0);
	}

	mp_->draw_meshes(viewMatrix);
	//screw[i]->setTransform(temp2);
	glutSwapBuffers();
};

//----------------------------------------------------------------------------

void mouse(int button, int state, int x, int y)
{
	mouse_x = x;
	mouse_y = y;
	glutPostRedisplay();
}

void mouseMotion(int x, int y)
{
	float x2, y2, z2;
	Helicopter->get_theta(x2, y2, z2);
	float x3 = x2 + (mouse_y - y) * 0.55;
	if (x3 > 30)x3 = 30;
	if (x3 < -85)x3 = -85;
	Helicopter->set_theta(x3, y2 + (mouse_x - x) * 0.55, z2);
	for(int i=0;i<2;i++)
	screw[i]->set_theta(x3, y2 + (mouse_x - x) * 0.55, z2);
	mouse_x = x;
	mouse_y = y;
	glutPostRedisplay();
}



//----------------------------------------------------------------------------

void
keyboard(unsigned char key, int mousex, int mousey)
{
	float x, y, z;
	switch (key) {
	case 'W':
	case 'w':
		KBeyeMove += vec4(0, 0.1, 0, 0);
		break;
	case 'S':
	case 's':
		KBeyeMove -= vec4(0, 0.1, 0, 0);
		break;
	case 'A':
	case 'a':
		KBeyeMove += vec4(0.1, 0, 0, 0);
		break;
	case 'D':
	case 'd':
		KBeyeMove -= vec4(0.1, 0, 0, 0);
		break;
	case 'Q':
	case 'q':
		radius += 0.1;
		break;
	case 'E':
	case 'e':
		radius -= 0.1;
		break;
	case ' ':
		if (!isDrop)
		{
			isDrop = true;
			boomDirection = vec3(-direction.x, -direction.y, -direction.z);

			saveTransform = boom->getTransform();
			t = 0;
			float x1, y1, z1;
			boom->get_translate(x1, y1, z1);
			high = y1;
			disX = x1;
			disZ = z1;
		}
		break;

	}
	glutPostRedisplay();
}
void dropWay()
{
	if (isDrop)
	{
		
		if (high > 0)
		{
			boom->setTransform(boom->getTransform()* Translate(0.5*boomDirection)* Translate(0.005 * vec3(0,-(++t),0)));
			boomDirection = normalize(0.5 * boomDirection + 0.005 * vec3(0, -t, 0));

			high=high + 0.5 * boomDirection.y - 0.005 *t;
			disX = disX + 0.5 * boomDirection.x;
			disZ = disZ + 0.5 * boomDirection.z;
		}
		
		else
		{
			for (int i = 0; i < ptcNum; i++)
			{
				ptc[i]->set_translate(disX, high - 0.5 * boomDirection.y + 0.01 * t, disZ);
			}
			float x1, y1, z1,x2,y2,z2;
			rosette->get_translate(x1, y1, z1);
			rosette->get_scale(x2, y2, z2);
			if (sqrt(pow(disX-x1, 2) + pow(disZ-z1, 2)) < x2)
			{
				score++;
				rosette->set_scale(x2*0.9, y2 * 0.9, z2 * 0.9);
				std::cout << "Hitted! SCORE:"<<score << std::endl;
			}
			else
				std::cout << "Overshot." << std::endl;
			sprTime = 10;
			boom->setTransform(saveTransform);
			isSpr = true;
			isDrop = false;
		}
		//boom->set_translate(next.x, next.y, next.z);
	}

}

void ptcPost()
{

	if (isSpr)
	{
		mat4 save[ptcNum];
		if(--sprTime)
			for (int i = 0; i < ptcNum; i++)
			{
				//srand(time(0));
				save[i] = ptc[i]->getTransform();
				vec3 randD = normalize(vec3(0.01 * (rand() % 100 - 50), 0.01 * (rand() % 100 - 50), 0.01 * (rand() % 100 - 50)));
				ptc[i]->setTransform(ptc[i]->getTransform() * 0.0001*Translate(randD));
			}

		else
		{
			for (int i = 0; i < ptcNum; i++)
			{
				ptc[i]->setTransform(save[i]);
				ptc[i]->set_translate(0, -999, 0);
			}
				
			isSpr = false;

		}

	}

}

void
idle(void)
{
	
	Sleep(10);

	glutPostRedisplay();
}

void moveItself(int value)
{
	float x1, y1, z1, x2, y2, z2;
	Helicopter->get_translate(x1, y1, z1);
	Helicopter->get_theta(x2, y2, z2);
	float y3 = y1 + sin(x2 * DegreesToRadians) * V;
	if (y3 < 1)y3 = 1;
	Helicopter->set_translate(x1 + sin(y2 * DegreesToRadians) * V, y3, z1 + cos(y2 * DegreesToRadians) * V);
	
	dropWay();
	ptcPost();
	glutTimerFunc(dt, moveItself, 1);
}

void special_key(int key, int x, int y) {

	static float x1, y1, z1, x2, y2, z2;

		switch (key) {
		case GLUT_KEY_UP:


			break;
		case GLUT_KEY_DOWN:


			break;
		/*case GLUT_KEY_LEFT:
			robot[TORSO]->get_theta(x2, y2, z2);
			robot[TORSO]->set_theta(x2, y2 + 10, z2);
			break;
		case GLUT_KEY_RIGHT:
			robot[TORSO]->get_theta(x2, y2, z2);
			robot[TORSO]->set_theta(x2, y2 - 10, z2);
			break;*/
		}

	glutPostRedisplay();
}



//----------------------------------------------------------------------------
int
main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(1024, 768);
	glutInitContextVersion(3, 2);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow("2017303031 钟善扬 期末大作业");
	glewExperimental = GL_TRUE;
	glewInit();
	init();


	mp_ = new Mesh_Painter;


	My_Mesh* floor = new My_Mesh;
	floor->generate_floor(-0.0001, -30, -30, 30, 30, 1);
	floor->set_texture_file("texture/shandi.jpg");
	floor->set_translate(0, 0, 0);
	floor->set_theta(0, 90, 0);
	floor->setShadow(false);
	my_meshs.push_back(floor);
	mp_->add_mesh(floor);


	Helicopter = new My_Mesh;
	Helicopter->load_obj("texture/Blackhawk_obj/uh60.obj", Scale(0.01, 0.01, 0.01) * mat4(1));
	Helicopter->set_texture_file("texture/hongbai.png");
	Helicopter->set_translate(2, 0.3, -2);
	Helicopter->set_theta(0, 5, 0);
	Helicopter->set_theta_step(0, 0, 0);
	Helicopter->setShadow(true);
	my_meshs.push_back(Helicopter);
	mp_->add_mesh(Helicopter);

	screw = new My_Mesh*[2];
	for (int i = 0; i < 2; i++)
	{
		screw[i] = new My_Mesh;
		screw[i]->generate_body();
		screw[i]->set_texture_file("texture/hongbai.png");
		screw[i]->setShadow(true);
		screw[i]->set_scale(1.5, 0.01, 0.035);
		screw[i]->set_theta(0, 0, 0);
		screw[i]->set_theta_step(0, 4, 0);
		my_meshs.push_back(screw[i]);
		mp_->add_mesh(screw[i]);
	}
	screw[1]->set_theta_step(0, 20, 0);

	boom = new My_Mesh;
	boom->load_obj("texture/Bomb_obj/Bomb.obj", Scale(1, 1, 1) * mat4(1));
	boom->set_texture_file("texture/heli.png");
	boom->set_translate(2, 0.3, -2);
	boom->set_theta(0, 5, 0);
	boom->set_theta_step(0, 0, 0);
	boom->setShadow(true);
	my_meshs.push_back(boom);
	mp_->add_mesh(boom);

	ptc = new My_Mesh * [ptcNum];
	for (int i = 0; i < ptcNum; i++)
	{
		ptc[i] = new My_Mesh;
		ptc[i]->generate_body();
		//ptc[i]->set_texture_file("texture/heli.png");
		ptc[i]->setShadow(false);
		ptc[i]->set_scale(100.0/ ptcNum, 100.0 / ptcNum, 100.0 / ptcNum);
		ptc[i]->set_texture_file("texture/heli.png");
		ptc[i]->set_theta(0, 0, 0);
		ptc[i]->set_theta_step(0, 0, 0);
		my_meshs.push_back(ptc[i]);
		mp_->add_mesh(ptc[i]);
	}

	rosette = new My_Mesh;
	rosette->load_obj("texture/shield_skandinav_medieval_02.obj", Scale(4, 4, 4) * mat4(1));
	rosette->set_texture_file("texture/hongbai.png");
	rosette->set_translate(0, 0, 0);
	rosette->set_theta(90, 0, 0);
	rosette->set_theta_step(0, 0, 0);
	rosette->setShadow(true);
	my_meshs.push_back(rosette);
	mp_->add_mesh(rosette);

	mp_->init_shaders("v_texture.glsl", "f_texture.glsl");
	mp_->setlightPos(lightPos);
	mp_->update_vertex_buffer();
	mp_->update_texture();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(mouseMotion);
	glutIdleFunc(idle);
	glutSpecialFunc(special_key);
	glutTimerFunc(dt, moveItself, 1);
	glutMainLoop();


	for (unsigned int i = 0; i < my_meshs.size(); i++)
	{
		delete my_meshs[i];
	}
	delete mp_;

	return 0;
}
