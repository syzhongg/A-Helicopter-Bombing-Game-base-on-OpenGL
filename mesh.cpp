#include "mesh.h"
#include<sstream>
#include <fstream>
#include <iosfwd>
#include <algorithm>
#include <gl/GL.h>
#include <math.h>
#include <algorithm>
#include <mat.h>

using namespace std;
My_Mesh::My_Mesh()
{
	Opoints = vec3(0, 0, 0);
	vTranslation[0] = Theta[0] = 0;
	vTranslation[1] = Theta[1] = 0;
	vTranslation[2] = Theta[2] = 0;
	Scale[0] = 1;
	Scale[1] = 1;
	Scale[2] = 1;
	for(int i=0;i<6;i++)
	Theta[i] = 0;
}


My_Mesh::~My_Mesh()
{

}


void My_Mesh::load_obj(std::string obj_File, mat4 transform)     //请在此添加代码实现对含有UV坐标的obj文件的读取
{
	ifstream ifs(obj_File);

	string s;
	float a, b, c, coa, cob, coc;
	unsigned int aa[4], bb[4], cc[4];
	int v[3], t[3], n[3];
	while (getline(ifs, s))
	{
		if (s.length() < 2) continue;
		if (s[0] == 'v' && s[1] == 't') { //纹理坐标 
			istringstream is(s);
			std::string st;
			is >> st >> a >> b >> c;
			m_vt_list_.push_back(a);
			m_vt_list_.push_back(b);
			m_vt_list_.push_back(c);
		}
		else if (s[0] == 'v' && s[1] == 'n') { //法线坐标 以及颜色 
			istringstream is(s);
			std::string st;
			is >> st >> a >> b >> c;
			m_normals_.push_back(a);
			m_normals_.push_back(b);
			m_normals_.push_back(c);
			normal_to_color(a, b, c, coa, cob, coc);
			m_color_list_.push_back(coa);
			m_color_list_.push_back(cob);
			m_color_list_.push_back(coc);
		}
		else if (s[0] == 'v') {    //顶点坐标 
			istringstream is(s);
			std::string st;
			is >> st >> a >> b >> c;
			vec4 result = transform * vec4(a, b, c, 1);
			m_vertices_.push_back(result.x);
			m_vertices_.push_back(result.y);
			m_vertices_.push_back(result.z);
		}
		else if (s[0] == 'f') {  //顶点索引 
			int i;
			for (i = 2; i < s.length(); i++)
				if (s[i] == '/') s[i] = ' ';
			string st;
			istringstream is(s);
			is >> st;
			for (i = 0; i < 3; i++) {
				is >> aa[i] >> bb[i] >> cc[i];
				m_faces_.push_back(aa[i] - 1);
				m_faces_.push_back(bb[i] - 1);
				m_faces_.push_back(cc[i] - 1);
			}
			is >> aa[3];
			if (is) {
				is >> bb[3] >> cc[3];
				m_faces_.push_back(aa[3] - 1);
				m_faces_.push_back(bb[3] - 1);
				m_faces_.push_back(cc[3] - 1);
				m_faces_.push_back(aa[0] - 1);
				m_faces_.push_back(bb[0] - 1);
				m_faces_.push_back(cc[0] - 1);
				m_faces_.push_back(aa[2] - 1);
				m_faces_.push_back(bb[2] - 1);
				m_faces_.push_back(cc[2] - 1);
			}
		}
	}

	float minX, maxX, minY, maxY, minZ, maxZ;
	minX = maxX = minY = maxY = minZ = maxZ = 0;
	for (unsigned int j = 0; j < this->m_vertices_.size(); j += 3) {
		if (m_vertices_[j] < minX) minX = m_vertices_[j];
		if (m_vertices_[j] > maxX) maxX = m_vertices_[j];
	}
	for (unsigned int j = 1; j < this->m_vertices_.size(); j += 3) {
		if (m_vertices_[j] < minY) minY = m_vertices_[j];
		if (m_vertices_[j] > maxY) maxY = m_vertices_[j];
	}
	for (unsigned int j = 2; j < this->m_vertices_.size(); j += 3) {
		if (m_vertices_[j] < minZ) minZ = m_vertices_[j];
		if (m_vertices_[j] > maxZ) maxZ = m_vertices_[j];
	}

	this->m_min_box_ = point3f(minX, minY, minZ);
	this->m_max_box_ = point3f(maxX, maxY, maxZ);

	this->m_center_ = point3f((minX + maxX) / 2, (minY + maxY) / 2, (minZ + maxZ) / 2);
};

void My_Mesh::normal_to_color(float nx, float ny, float nz, float& r, float& g, float& b)
{
	r = float(std::min(std::max(0.5 * (nx + 1.0), 0.0), 1.0));
	g = float(std::min(std::max(0.5 * (ny + 1.0), 0.0), 1.0));
	b = float(std::min(std::max(0.5 * (nz + 1.0), 0.0), 1.0));
};

const VtList& My_Mesh::get_vts()
{
	return this->m_vt_list_;
};
void My_Mesh::clear_data()
{
	m_vertices_.clear();
	m_normals_.clear();
	m_faces_.clear();
	m_color_list_.clear();
	m_vt_list_.clear();
};
void My_Mesh::get_boundingbox(point3f& min_p, point3f& max_p) const
{
	min_p = this->m_min_box_;
	max_p = this->m_max_box_;
};
const STLVectorf& My_Mesh::get_colors()
{
	return this->m_color_list_;
};
const VertexList& My_Mesh::get_vertices()
{
	return this->m_vertices_;
};
const NormalList& My_Mesh::get_normals()
{
	return this->m_normals_;
};
const FaceList& My_Mesh::get_faces()
{
	return this->m_faces_;
};

int My_Mesh::num_faces()
{
	return this->m_faces_.size() / 9;
};
int My_Mesh::num_vertices()
{
	return this->m_vertices_.size() / 3;
};

const point3f& My_Mesh::get_center()
{
	return this->m_center_;
};

void My_Mesh::generate_floor(float height, float x, float y, float dx, float dy, int num) {     //地面
	this->clear_data();
	this->m_center_ = point3f(0, 0, 0);
	unsigned int k = 0;
	int n = 0;
	float xl = dx - x;
	float yl = dy - y;
	for (float xx = x; xx <= dx; xx += 1) { 
		for (float yy = y; yy <= dy; yy += 1) {
			vec3 points[6] = {
				vec3(xx - 1,height,yy + 1),
				vec3(xx + 1,height,yy + 1),
				vec3(xx + 1,height,yy - 1),
				vec3(xx - 1,height,yy - 1),
				vec3(xx - 1,height,yy + 1),
				vec3(xx + 1,height,yy + 1),
			};
			for (int i = 0; i < 6; i++) {
				m_vertices_.push_back(points[i].x);
				m_vertices_.push_back(points[i].y);
				m_vertices_.push_back(points[i].z);
				m_normals_.push_back(0);
				m_normals_.push_back(1.0);
				m_normals_.push_back(0);
				m_color_list_.push_back(1);
				m_color_list_.push_back(1);
				m_color_list_.push_back(1);
				m_vt_list_.push_back(num * (points[i].z - y - 1) / yl);
				m_vt_list_.push_back(num * (points[i].x - x - 1) / xl);
				m_vt_list_.push_back(0);
			}
			for (n = 0; n < 3; n++) m_faces_.push_back(k);
			k++;
			for (n = 0; n < 3; n++) m_faces_.push_back(k);
			k++;
			for (n = 0; n < 3; n++) m_faces_.push_back(k);
			k++;
			for (n = 0; n < 3; n++) m_faces_.push_back(k);
			k++;
			for (n = 0; n < 3; n++) m_faces_.push_back(k);
			k++;
			for (n = 0; n < 3; n++) m_faces_.push_back(k);
			k++;
		};
	}
}

void My_Mesh::generate_body() {
	vec4 center = vec4(0, 0, 0, 1);
	vec4 m_min = transform * vec4(-0.5, -0.5, -0.5, 1);
	vec4 m_max = transform * vec4(0.5, 0.5, 0.5, 1);
	int i;
	this->m_center_ = point3f(0, 0, 0);
	this->m_min_box_ = point3f(-0.5, -0.5, -0.5);
	this->m_max_box_ = point3f(0.5, 0.5, 0.5);

	vec3 vertices[8] = {
		vec3(-0.5, 0.5, 0.5),   //0
		vec3(0.5, 0.5, 0.5),	//1
		vec3(0.5, -0.5, 0.5),	//2
		vec3(-0.5, -0.5, 0.5),	//3
		vec3(-0.5, -0.5, -0.5),	//4
		vec3(0.5, -0.5, -0.5),	//5
		vec3(0.5, 0.5, -0.5),	//6
		vec3(-0.5, 0.5, -0.5)	//7
	};
	for (i = 0; i < 8; i++) {
		m_vertices_.push_back(vertices[i].x);
		m_vertices_.push_back(vertices[i].y);
		m_vertices_.push_back(vertices[i].z);
	}
	vec3 normals[6] = {
		vec3(0, 0, 1),
		vec3(1, 0, 0),
		vec3(0, 0, -1),
		vec3(-1,0, 0),
		vec3(0, 1, 0),
		vec3(0, -1, 0)
	};
	for (i = 0; i < 6; i++) {
		m_normals_.push_back(normals[i].x);
		m_normals_.push_back(normals[i].y);
		m_normals_.push_back(normals[i].z);
		m_color_list_.push_back(0.01 * (rand() % 50 + 50));
		m_color_list_.push_back(0.01 * (rand() % 50 + 50));
		m_color_list_.push_back(0.01 * (rand() % 50 + 50));
	}
	const int indexes[12][3] = {
		{0,1,2},{3,0,2},
		{1,6,5},{2,1,5},
		{6,7,4},{5,6,4},
		{7,0,3},{4,7,3},
		{7,6,1},{0,7,1},
		{4,5,2},{3,4,2}
	};
	m_vt_list_.push_back(0.5);
	m_vt_list_.push_back(0.5);
	m_vt_list_.push_back(0);

	for (int i = 0; i < 12; i++) {
		for (int j = 0; j < 3; j++) {
			m_faces_.push_back(indexes[i][j]);
			m_faces_.push_back(0);
			m_faces_.push_back(i / 2);
		}
	}
}

void My_Mesh::set_texture_file(std::string s)
{
	this->texture_file_name = s;
};
std::string My_Mesh::get_texture_file()
{
	return this->texture_file_name;
};

void My_Mesh::set_translate(float x, float y, float z)
{
	vTranslation[0] = x;
	vTranslation[1] = y;
	vTranslation[2] = z;

};
void My_Mesh::get_translate(float& x, float& y, float& z)
{
	x = vTranslation[0];
	y = vTranslation[1];
	z = vTranslation[2];
};
void My_Mesh::set_translatexz(float x, float z) {
	vTranslation[0] = x;
	vTranslation[2] = z;
}
void My_Mesh::set_theta(float x, float y, float z)
{
	Theta[0] = x;
	Theta[1] = y;
	Theta[2] = z;
};
void My_Mesh::get_theta(float& x, float& y, float& z)
{
	x = Theta[0];
	y = Theta[1];
	z = Theta[2];
}

void My_Mesh::get_scale(float& x, float& y, float& z)
{
	x = Scale[0];
	y = Scale[1];
	z = Scale[2];
};
void My_Mesh::set_scale(float x, float y, float z)
{
	this->Scale[0] = x;
	this->Scale[1] = y;
	this->Scale[2] = z;
};
void My_Mesh::set_theta_step(float x, float y, float z)
{
	Theta_step[0] = x;
	Theta_step[1] = y;
	Theta_step[2] = z;
};

void My_Mesh::set_theta345(float x, float y, float z)
{
	Theta_step[3] = x;
	Theta_step[4] = y;
	Theta_step[5] = z;
};

void My_Mesh::add_theta_step()
{
	Theta[3] = Theta[3] + Theta_step[0];
	Theta[4] = Theta[4] + Theta_step[1];
	Theta[5] = Theta[5] + Theta_step[2];
};

void My_Mesh::get_theta_step(float& x, float& y, float& z)
{
	x = Theta[3];
	y = Theta[4];
	z = Theta[5];
};

mat4& My_Mesh::getTransform() {
	return this->transform;
};

void My_Mesh::setTransform(mat4 transform) {
	this->transform = transform;
};
void My_Mesh::setShadow(bool flag) {
	this->m_shadow_ = flag;
};

bool My_Mesh::isShadow() {
	return this->m_shadow_;
};
