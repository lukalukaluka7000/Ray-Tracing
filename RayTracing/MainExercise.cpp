#include "MainExercise.h"
#include<iostream>
//#include<GL/glew.h>
#include<GL/glut.h>
#include<memory>
#include<glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include<vector>
#include <limits>
#include<algorithm>
/// --- GLOBALS ---
float PSI = 0.0001;
class Light {
public:
	Light() {};
	Light(glm::vec3 Position, glm::vec3 Color) {
		position = Position; color = Color;
	}
	glm::vec3 getPosition() { return position; }
	glm::vec3 getColor() { return color; }
private:
	glm::vec3 position;
	glm::vec3 color;
};
class Zraka {
public:
	Zraka() {};
	Zraka(glm::vec3 vectorV, glm::vec3 Direction) {
		d = Direction;
		v = vectorV + glm::vec3(PSI) * d;
	}
	glm::vec3 getPointParametrized(float lambda) {
		return v + glm::vec3(lambda) * d;
	}
	glm::vec3 getV() { return v; }
	glm::vec3 getd() { return d; }
private:
	glm::vec3 v = glm::vec3(0.0f);
	glm::vec3 d = glm::vec3(0.0f);
};
class SceneObject {
public:
	SceneObject() {};

	virtual void init(short ID, glm::vec3 Center, float Radius, glm::vec3 amb, glm::vec3 dif, glm::vec3 ref, float n, float kref) = 0;
	virtual void init(short ID, glm::vec3 Center, glm::vec3 V1, glm::vec3 V2, float Wi, float He, 
		glm::vec3 amb, glm::vec3 dif, glm::vec3 ref, float n, float kref,
		glm::vec3 Bamb, glm::vec3 Bdif, glm::vec3 Bref, float Bn, float Bkref) = 0;
	virtual float izracunajSjecisteZrakeiObjekta(Zraka zraka/*, Kugla o*/) = 0;
	virtual glm::vec3 izracunajSjecisteZrakeiObjekta(Zraka zraka/*, Kugla o*/, float& lamb, float& mi) = 0;
	virtual glm::vec3 getPointOnPlane(float lamb, float mi) = 0;
	virtual glm::vec3 getNormalInPoint(glm::vec3 point) = 0;
	virtual glm::vec3 getCenter() = 0;
	virtual short getID() = 0;
	virtual glm::vec3 getNormal() = 0;

	void setAmbient(glm::vec3 amb) {
		ambient = amb;
	}
	void setDiffuse(glm::vec3 diff) {
		diffuse = diff;
	}
	void setReflective(glm::vec3 ref) {
		reflective = ref;
	}
	void setN(float N) { n = N; }
	void setKref(float KREF) { kref = KREF; }

	glm::vec3 getAmb() { return ambient; }
	glm::vec3 getDif() { return diffuse; }
	glm::vec3 getRef() { return reflective; }
	float		getN() { return n; }
	float	 getKref() { return kref; }
	bool front;

	virtual glm::vec3 getBAmb() = 0;
	virtual glm::vec3 getBDif() =0;
	virtual glm::vec3 getBRef() =0;
	virtual float		getBN() =0;
	virtual float	 getBKref() =0;
private:
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 reflective;
	float n;
	float kref;
};

class Kugla : public SceneObject {
public:
	Kugla() { 
		id = 1; center = glm::vec3(0.0f); radius = 0.0f;
	};

	virtual void init(short ID, glm::vec3 Center, float Radius, glm::vec3 amb, glm::vec3 dif, glm::vec3 ref, float n, float kref) override{
		id = ID; center = Center; radius = Radius;
		setAmbient(amb);
		setDiffuse(dif);
		setReflective(ref);
		setN(n);
		setKref(kref);
	}
	virtual void init(short ID, glm::vec3 Center, glm::vec3 V1, glm::vec3 V2, float Wi, float He, 
		glm::vec3 amb, glm::vec3 dif, glm::vec3 ref, float n, float kref,
		glm::vec3 Bamb, glm::vec3 Bdif, glm::vec3 Bref, float Bn, float Bkref) {
		std::cout << "init kugla" << std::endl;
	}
	virtual glm::vec3 izracunajSjecisteZrakeiObjekta(Zraka zraka/*, Kugla o*/, float& lamb, float& mi)override {
		std::cout << "izracunajSjecisteZrakeiObjekta kugla" << std::endl;

		return glm::vec3(0.0f);
	}
	virtual float izracunajSjecisteZrakeiObjekta(Zraka zraka/*, Kugla o*/)  override{
		glm::vec3 Ts = zraka.getV(); glm::vec3 d = zraka.getd();

		// RJESAVANJE KVADRATNE JEDNADZBE
		float a = 1.0f;
		float b = glm::dot(glm::vec3(2.0f) * d, Ts - getCenter());

		float norma = glm::length(Ts - getCenter());
		float c = norma * norma - pow(getRadius(), 2);

		float discriminant = b * b - 4 * a * c;
		if (discriminant > 0) {
			float lambda1 = (-b + sqrt(discriminant)) / (2.0f * a);
			float lambda2 = (-b - sqrt(discriminant)) / (2.0f * a);
			//ISTI SU, PROVJERI KOJI JE MANJI(BLIZI), ZAMIJENI IM VRIJEDNOSTI
			if (lambda1 > 0 && lambda2 > 0) {
				if (lambda1 > lambda2) {
					float temp = lambda1;
					lambda1 = lambda2;
					lambda2 = temp;
				}
				return lambda1;
			}
			//PROMATRAC SE NALAZI U SFERI, VRACAM DAKLE ONO DRUGO PRESJECISTE
			if ((lambda1 <= 0 && lambda2 > 0) || (lambda1 >= 0 && lambda2 < 0)) {
				if (lambda1 > lambda2) {
					float temp = lambda1;
					lambda1 = lambda2;
					lambda2 = temp;
				}
				return lambda2;
			}
		}
		else if (discriminant == 0) {
			float lambda = (-b + sqrt(discriminant)) / (2.0f * a);
			if (lambda < 0) {
				return std::numeric_limits<float>::max(); // tocka je iza promatraca, pa vracam infinity
			}
			else if (lambda > 0) {
				return lambda;
			}
		}
		else {
			float realPart = -b / (2.0f * a);
			float imagPart = sqrt(-discriminant) / (2.0f * a);
			return std::numeric_limits<float>::max();
		}

		return std::numeric_limits<float>::max();
	}
	virtual glm::vec3 getPointOnPlane(float lamb, float mi) override{
		std::cout << "getPointOnPlane kugla" << std::endl;

		return glm::vec3(0.0f);
	}
	virtual glm::vec3 getNormalInPoint(glm::vec3 tockaNaKugli) override {
		return glm::normalize(tockaNaKugli - center);
	}

	virtual glm::vec3 getCenter() override {
		return center;
	}
	virtual glm::vec3 getNormal() override {
		std::cout << "getNormal kugla" << std::endl;

		return glm::vec3(0.0f);
	}

	glm::vec3 getBAmb() override { return glm::vec3(); }
	glm::vec3 getBDif() override { return glm::vec3(); }
	glm::vec3 getBRef() override { return glm::vec3(); }
	float		getBN() override { return 0.0f; }
	float	 getBKref() override { return 0.0f; }

	float getRadius() { return radius; }
	short getID() { return id; }

private:
	short id = 0;
	glm::vec3 center;
	float radius;
};
class Krpica : public SceneObject {
public:
	Krpica() {
		id = 2; center = glm::vec3(0.0f); v1 = glm::vec3(0.0f); v2 = glm::vec3(0.0f); normal = glm::vec3(0.0f); wi = 0.0f; he = 0.0f;
	};
	//objects.back()->init(center, v1, v2, wi, he);
	virtual void init(short ID, glm::vec3 Center, float Radius, glm::vec3 amb, glm::vec3 dif, glm::vec3 ref, float n, float kref) {
		std::cout << "init  krpica" << std::endl;

	}
	
	virtual void init(short ID, glm::vec3 Center, glm::vec3 V1, glm::vec3 V2, float Wi, float He, 
		glm::vec3 amb, glm::vec3 dif, glm::vec3 ref, float n, float kref,
		glm::vec3 Bamb, glm::vec3 Bdif, glm::vec3 Bref, float Bn, float Bkref) override{
		id = ID;  center = Center; v1 = V1; v2 = V2; wi = Wi; he = He;
		setAmbient(amb);
		setDiffuse(dif);
		setReflective(ref);
		setN(n);
		setKref(kref);

		setBAmbient(Bamb);
		setBDiffuse(Bdif);
		setBReflective(Bref);
		setBN(Bn);
		setBKref(Bkref);

		normal = glm::cross(V1, V2);
	}
	void CramersRule(glm::mat3x3 A, glm::vec3 b, glm::vec3 d, float& lamb, float& mi, float& ep) {
		/*float A_data[16] = {
		   2, 1, 1,
		   1, 2, 1,
		   1, 1, 2 };
		A = glm::make_mat3x3(A_data);
		float D = glm::determinant(A);

		float d1_data[16] = {
		   2, 1, 1,
		   3, 2, 1,
		  -1, 1, 2 };
		glm::mat3x3 d1 = glm::make_mat3x3(d1_data);
		float d2_data[16] = {
		   2, 2, 1,
		   1, 3, 1,
		   1, -1, 2 };
		glm::mat3x3 d2 = glm::make_mat3x3(d2_data);
		float d3_data[16] = {
		   2, 1, 2,
		   1, 2, 3,
		   1, 1, -1 };
		glm::mat3x3 d3 = glm::make_mat3x3(d3_data);

		float D1 = glm::determinant(d1);
		float D2 = glm::determinant(d2);
		float D3 = glm::determinant(d3);

		if (D != 0) {
			lamb = D1 / D;
			mi = D2 / D;
			ep = D3 / D;
		}
		else {
			if (D1 == 0 && D2 == 0 && D3 == 0) {
				printf("Infinite solutions\n");
			}
			else if (D1 != 0 || D2 != 0 || D3 != 0) {
				printf("No solutions\n");
			}
			lamb = D1 / D;
			mi = D2 / D;
			ep = D3 / D;
		}*/




		float D = glm::determinant(A);

		float d1_data[16] = {
		   b.x, v2.x, -d.x,
		   b.y, v2.y, -d.y,
		   b.z, v2.z, -d.z };
		glm::mat3x3 d1 = glm::make_mat3x3(d1_data);
		float d2_data[16] = {
		   v1.x, b.x, -d.x,
		   v1.y, b.y, -d.y,
		   v1.z, b.z, -d.z };
		glm::mat3x3 d2 = glm::make_mat3x3(d2_data);
		float d3_data[16] = {
		   v1.x, v2.x, b.x,
		   v1.y, v2.y, b.y,
		   v1.z, v2.z, b.z };
		glm::mat3x3 d3 = glm::make_mat3x3(d3_data);

		float D1 = glm::determinant(d1);
		float D2 = glm::determinant(d2);
		float D3 = glm::determinant(d3);

		if (D != 0) {
			lamb = D1 / D;
			mi = D2 / D;
			ep = D3 / D;
		}
		else {
			if (D1 == 0 && D2 == 0 && D3 == 0) {
				//printf("Infinite solutions\n");
			}
			else if (D1 != 0 || D2 != 0 || D3 != 0) {
				//printf("No solutions\n");
			}
			lamb = D1 / D;
			mi = D2 / D;
			ep = D3 / D;
			lamb = std::numeric_limits<float>::max();
			mi = std::numeric_limits<float>::max();
		}
	}

	virtual float izracunajSjecisteZrakeiObjekta(Zraka zraka/*, Kugla o*/) override {
		std::cout << "izracunajSjecisteZrakeiObjekta  krpica" << std::endl;

		return 0.0f;
	}
	virtual glm::vec3 izracunajSjecisteZrakeiObjekta(Zraka zraka/*, Kugla o*/, float& lamb, float&mi) override{
		// ~p(λ,μ) =~c+λ·~v1+μ·~v2
		glm::vec3 s = zraka.getV();
		glm::vec3 c = getCenter();
		glm::vec3 d = zraka.getd();

		float A_data[16] = {
		   v1.x, v2.x, -d.x,
		   v1.y, v2.y, -d.y, 
		   v1.z, v2.z, -d.z };
		glm::mat3x3 A = glm::make_mat3x3(A_data);

		glm::vec3 b(s.x - c.x, s.y - c.y, s.z - c.z);

		float Lambda, Mi, Epsilon;
		CramersRule(A, b, d, Lambda, Mi, Epsilon);


		// SJECISTE ODBACITI JER  JE IZA NAS I MI GA NE VIDIMO
		if (Epsilon < 0) {
			lamb = std::numeric_limits<float>::max();
			mi = std::numeric_limits<float>::max();
			return glm::vec3(0.0f);
		}
		else {
			if (Lambda >= -wi / 2 && Lambda <= wi / 2 && Mi >= -he / 2 && Mi <= he / 2) {
				//SJECISTE POSTOJI
				lamb = Lambda; 
				mi = Mi;
				glm::vec3 tockaNaRavnini = c + glm::vec3(lamb) * v1 + glm::vec3(mi) * v2;
				return tockaNaRavnini;
			}
			else {
				return glm::vec3(0.0f);
			}
		}
		return glm::vec3(0.0f);
	}
	glm::vec3 getPointOnPlane(float lamb, float mi) {
		glm::vec3 pointP = center + glm::vec3(lamb) * v1 + glm::vec3(mi) * v2;
		return pointP;
	}
	virtual glm::vec3 getNormalInPoint(glm::vec3 point) override {
		return glm::normalize(glm::cross(v1, v2)); // smjer u kojem ovako izracunata normalla pokazuje
	}
	virtual glm::vec3 getCenter() override {
		return center;
	}
	short getID() override { return id; }
	glm::vec3 getNormal() override {
		return normal;
	}


	void setBAmbient(glm::vec3 Bamb) {
		Bambient = Bamb;
	}
	void setBDiffuse(glm::vec3 Bdiff) {
		Bdiffuse = Bdiff;
	}
	void setBReflective(glm::vec3 Bref) {
		Breflective = Bref;
	}
	void setBN(float BN) { Bn = BN; }
	void setBKref(float BKREF) { Bkref = BKREF; }
	glm::vec3 getBAmb() override { return Bambient; }
	glm::vec3 getBDif() override { return Bdiffuse; }
	glm::vec3 getBRef() override { return Breflective; }
	float		getBN() override { return Bn; }
	float	 getBKref() override { return Bkref; }

private:
	short id = 0;
	glm::vec3 center;
	glm::vec3 v1;
	glm::vec3 v2;
	glm::vec3 normal;
	float wi;
	float he;

	glm::vec3 Bambient;
	glm::vec3 Bdiffuse;
	glm::vec3 Breflective;
	float Bn;
	float Bkref;

	
};

unsigned int currentStateFlag;
glm::vec3 O(12.0f, 4.0f, 9.0f);
glm::vec3 G(-1.0f, -1.0, -0.5);
glm::vec3 Vup(0.0, 0.0, 1.0);
//glm::vec3 O(4.0f, 4.0f, 8.0f);
//glm::vec3 G(-1.0f, -1.0, -0.5);
//glm::vec3 Vup(0.0, 1.0, 0.0);
float H = 3;
float l = sqrt(3); 
float r = sqrt(3);
float b = sqrt(3);
float t = sqrt(3);
glm::vec3 ga(0.1, 0.1, 0.1); // globalna ambijentalna svjetlost

std::vector<SceneObject*> objects;

std::vector<Light> lights;
float SW, SH;
int dubinaRekurzije = 16;

/// ---------------
glm::vec3 slijedi(Zraka);
glm::vec3 utvrdiBoju(SceneObject*, Zraka, float);

void MainExercise::init(int windowWidth, int windowHeight) {
	_windowWidth = windowWidth;
	_windowHeight = windowHeight;

	SW = windowWidth; SH = windowHeight;
	//currentStateFlag |= ProgramMode::GETTING_POINTS;
}
void crtajOsi() {
	float scale = 1.5f;

	glColor4f(0.8, 0.3, 0.3, 0.3);
	glLineWidth(3);
	glBegin(GL_LINES);
	glVertex3f(-2.0, 0.0, 0.0);
	glVertex3f(4.0, 0.0, 0.0);
	glEnd();

	glColor4f(0.3, 0.8, 0.3, 0.3);
	glLineWidth(3);
	glBegin(GL_LINES);
	glVertex3f(0.0, -2.0, 0.0);
	glVertex3f(0.0, 4.0, 0.0);
	glEnd();

	glColor4f(0.2, 0.3, 0.8, 0.3);
	glLineWidth(3);
	glBegin(GL_LINES);
	glVertex3f(0.0, 0.0, -2.0);
	glVertex3f(0.0, 0.0, 4.0);
	glEnd();

}
void kocka(float w) {
	float wp = w / 2.0f;
	glBegin(GL_LINE_LOOP);
	glVertex3f(-wp, -wp, wp); 
	glVertex3f(wp, -wp, wp);
	glVertex3f(wp, wp, wp);
	glVertex3f(-wp, wp, wp);
	glEnd();
	
	glBegin (GL_LINE_LOOP);
	glVertex3f(-wp, wp, -wp);
	glVertex3f(wp, wp, -wp);
	glVertex3f(wp, -wp, -wp);
	glVertex3f(-wp, -wp, -wp);
	glEnd();
	
	glBegin(GL_LINE_LOOP);
	glVertex3f(wp, wp, -wp);
	glVertex3f(-wp, wp, -wp);
	glVertex3f(-wp, wp, wp);
	glVertex3f(wp, wp, wp);
	glEnd();
	
	glBegin(GL_LINE_LOOP);
	glVertex3f(wp, -wp, wp);
	glVertex3f(-wp, -wp, wp);
	glVertex3f(-wp, -wp, -wp);
	glVertex3f(wp, -wp, -wp);
	glEnd();
	
	glBegin(GL_LINE_LOOP);
	glVertex3f(wp, -wp, -wp);
	glVertex3f(wp, wp, -wp);
	glVertex3f(wp, wp, wp);
	glVertex3f(wp, -wp, wp);
	glEnd();
	
	glBegin(GL_LINE_LOOP);
	glVertex3f(-wp, -wp, wp);
	glVertex3f(-wp, wp, wp);
	glVertex3f(-wp, wp, -wp);
	glVertex3f(-wp, -wp, -wp);
	glEnd();
}
void kocka1() {
	kocka(1.0f);
}
void krpica() {

	//glBegin(GL_POLYGON);
	//glColor3f(1.0f, 0.0f, 0.0f);
	//glVertex3f(5, 7, -2);
	//glColor3f(0.0f, 1.0f, 0.0f);
	//glVertex3f(4, -4, -2);
	//glColor3f(0.0f, 0.0f, 1.0f);
	////glVertex3f(-1, 0, -1);
	//glVertex3f(-4, -4, -2);
	//glColor3f(1.0f, 1.0f, 0.0f);
	//glVertex3f(-4, 4, -2);
	//glEnd();

	glBegin(GL_POLYGON);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(5, 7, -2);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(4, -4, -2);
	glColor3f(0.0f, 0.0f, 1.0f);
	//glVertex3f(-1, 0, -1);
	glVertex3f(-4, -4, -2);
	glColor3f(1.0f, 1.0f, 0.0f);
	glVertex3f(-4, 4, -2);
	glEnd();
	crtajOsi();

	glBegin(GL_POLYGON);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(3, -4, -5);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(3,-4,6);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(-5,-4,6);
	glColor3f(1.0f, 1.0f, 0.0f);
	glVertex3f(-5,-4,-5);
	glEnd();
}
void kugla(glm::vec3 Cent, float R) {
	glBegin(GL_POINTS);
	for (int x = -R; x <= R; x++) {
		for (int y = -R; y <= R; y++) {
			float z = sqrt(pow(R, 2) - (pow(x, 2) + pow(y, 2)));
			glVertex3i(x, y, int(z));
		}
	}
	glEnd();
}

void getNearPlaneInfo(glm::vec3& i, glm::vec3& j, glm::vec3& GuRavnini) {
	glm::vec3 smjerPremaGledistu = glm::normalize(G - O);
	GuRavnini = O + H * smjerPremaGledistu;

	i = glm::normalize(glm::cross(Vup, O - GuRavnini));
	j = glm::normalize(glm::cross(O - G, i));
}


void initObjects() {
	glm::vec3 center(2.0f, 2.0f, 2.0f); // plava
	glm::vec3 amb(0.0f, 0.4f, 0.8f);
	glm::vec3 dif(0.0f, 0.4f, 0.8f);
	glm::vec3 ref(0.3f, 0.3f, 0.3f);
	float n = 96; float kref = 0.2f;
	objects.push_back(new Kugla());
	objects.back()->init(1, center, 1.0f, amb, dif, ref, n, kref);

	center = glm::vec3(6.0f, 3.0f, 0.0f); // cyan
	amb = glm::vec3(0.0f, 0.6f, 0.6f);
	dif = glm::vec3(0.0f, 0.6f, 0.6f);
	ref = glm::vec3(0.3f, 0.3f, 0.3f);
	n = 96; kref = 0.2f;
	objects.push_back(new Kugla());
	objects.back()->init(1, center, 2.0f, amb, dif, ref, n, kref);

	center = glm::vec3(2.0f, 6.0f, 5.0f); // zuta
	amb = glm::vec3(0.7f, 0.7f, 0.0f);
	dif = glm::vec3(0.7f, 0.7f, 0.0f);
	ref = glm::vec3(0.3f, 0.3f, 0.3f);
	n = 16; kref = 0.2f;
	objects.push_back(new Kugla());
	objects.back()->init(1, center, .5f, amb, dif, ref, n, kref);

	// OKOMITA
	center = glm::vec3(-8.0f, 0.0f, 2.0f);
	glm::vec3 v1(0.0f, 1.0f, 0.0f);
	glm::vec3 v2(0.0f, 0.0f, 1.0f);
	float wi = 20;float  he = 20;
	amb = glm::vec3(1.0f, 1.0f, 1.0f); // cisto bijelo ogledalo
	dif = glm::vec3(0.1f, 0.0f, 0.0f);
	ref = glm::vec3(0.0f, 0.0f, 0.0f);
	n = 32; kref = 0.7f; // puna reflesija ko ogledalo
	
	glm::vec3 Bamb = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 Bdif = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 Bref = glm::vec3(0.3f, 0.3f, 0.3f);
	float Bn = 32; float Bkref = 1.0f;
	objects.push_back(new Krpica());
	objects.back()->init(2, center, v1, v2, wi, he, 
						amb, dif, ref, n, kref,
						Bamb,Bdif,Bref,Bn,Bkref);

	// VODORAVNA
	center = glm::vec3(0.0f, 0.0f, -2.0f);
	v1 = glm::vec3(1.0f, 0.0f, 0.0f);
	v2 = glm::vec3(0.0f, 1.0f, 0.0f);
	wi = 16; he = 16;
	amb = glm::vec3(0.6f, 0.2f, 0.2f); // cisto bijelo ogledalo
	dif = glm::vec3(0.6f, 0.2f, 0.2f); // 
	ref = glm::vec3(0.3f, 0.3f, 0.3f); // blaga refleksija
	n = 32; kref = 0.0f; // puna reflesija ko ogledalo

	Bamb = glm::vec3(1.0f, 1.0f, 1.0f);
	Bdif = glm::vec3(1.0f, 1.0f, 1.0f);
	Bref = glm::vec3(0.3f, 0.3f, 0.3f);
	Bn = 32; Bkref = 0.0f;
	objects.push_back(new Krpica());
	objects.back()->init(2, center, v1, v2, wi, he,
						amb, dif, ref, n, kref,
						Bamb, Bdif, Bref, Bn, Bkref);


	/*glm::vec3 lightPos(2.0f, 3.0f, 5.0f);
	glm::vec3 lightCol(0.51f, 0.51f, 0.51f);
	lights.push_back(Light(lightPos, lightCol));

	lightPos = glm::vec3(-2.0f, 3.0f, 5.0f);
	lightCol = glm::vec3(0.5f, 0.5f, 0.5f);
	lights.push_back(Light(lightPos, lightCol));*/

	glm::vec3 lightPos(0.0f, -10.0f, 1.0f);
	glm::vec3 lightCol(0.1f, 0.1f, 0.1f);
	lights.push_back(Light(lightPos, lightCol));

	lightPos= glm::vec3(10.0f, 10.0f, 10.0f);
	lightCol= glm::vec3(0.5f, 0.5f, 0.5f);
	lights.push_back(Light(lightPos, lightCol));

	lightPos = glm::vec3(10.0f, 0.0f, 10.0f);
	lightCol = glm::vec3(0.5f, 0.5f, 0.5f);
	lights.push_back(Light(lightPos, lightCol));
}

glm::vec3 obojajPhong(SceneObject* k, glm::vec3 tockaNaKugli, Light light) {
	glm::vec3 difInt, refInt, ambInt;
	float N;
	if (k->getID() == 2 && k->front == false) {
		difInt = k->getBDif();
		refInt = k->getBRef();
		N = k->getBN();
		ambInt = k->getBAmb();
	}
	else {
		difInt = k->getDif();
		refInt = k->getRef();
		N = k->getN();
		ambInt = k->getAmb();
	}

	// diffuse Light
	glm::vec3 normalaNaObjektu = k->getNormalInPoint(tockaNaKugli);
	glm::vec3 lightVector = glm::normalize(light.getPosition() - tockaNaKugli);
	
	float brightness = dot(lightVector, normalaNaObjektu);
	//glm::vec3 diffuseLight(0, 0, brightness);
	glm::vec3 diffuseLight = std::max(brightness,0.0f) * difInt;

	// specular Light
	glm::vec3 reflectedLightVector = glm::reflect(-lightVector, normalaNaObjektu);
	glm::vec3 ocisteVektor = glm::normalize(O - tockaNaKugli);
	float spec = std::max(glm::dot(reflectedLightVector, ocisteVektor),0.0f); // r x v
	if (spec > 0 && k->getID() == 2) {
		//std::cout << "aaa" << std::endl;
	}
	spec = pow(spec, N);
	//glm::vec3 specularLight(spec, 0, 0);
	glm::vec3 specularLight = spec * refInt;

	//return k->getAmb() + glm::clamp(diffuseLight, 0.0f, 1.0f) + glm::clamp(specularLight, 0.0f, 1.0f);/*glm::clamp(diffuseLight,0.0f,1.0f)*/;
	return /*light.getColor() +*/ 
		0.2f*ambInt 
		+ glm::clamp(diffuseLight, 0.0f, 1.0f)
		+ glm::clamp(specularLight, 0.0f, 1.0f);/**/;
	
}
int dubinaRek = 0;
glm::vec3 utvrdiBoju(SceneObject* k, Zraka r, float lambda, float najmanjalamb, float najmanjimi) {
	//std::cout << dubinaRek << std::endl;
	glm::vec3 pointWorld(0.0f);
	if (k->getID() == 1) {
		pointWorld = r.getPointParametrized(lambda);
	}
	else if (k->getID() == 2) {
		pointWorld = k->getPointOnPlane(najmanjalamb, najmanjimi);
	}
	
	glm::vec3 color = glm::vec3(0.0f);// black
	
	for (auto& light : lights) {

		// TREBA MI ZRAKA OD pointWorld DO IZVORA
		Zraka premaIzvoru(pointWorld, glm::normalize(light.getPosition() - pointWorld));

		// PRONACI NAJBLIZE SJECISTE TE ZRAKE SA SVIM OBJEKTIMA U SCENI
		float najmanjaLambda = std::numeric_limits<float>::max();
		float najmanjalamb = std::numeric_limits<float>::max();
		float najmanjimi = std::numeric_limits<float>::max();

		//nije mi bitno koji je to objekt
		for (auto& obj : objects) {
			if (obj->getID() == 1) {
				float lambda = obj->izracunajSjecisteZrakeiObjekta(premaIzvoru);//izracunajSjecisteZrakeiKugle(premaIzvoru, obj);
				najmanjaLambda = std::min(najmanjaLambda, lambda);
				
			}
			if (obj->getID() == 2) {
				float lamb = std::numeric_limits<float>::max();
				float mi = std::numeric_limits<float>::max();
				obj->izracunajSjecisteZrakeiObjekta(premaIzvoru, lamb, mi);
				najmanjalamb = std::min(najmanjalamb, lamb);
				najmanjimi = std::min(najmanjimi, mi);
			}
		}
		//ako postoji sjeciste
		if (najmanjaLambda != std::numeric_limits<float>::max() || 
			najmanjalamb != std::numeric_limits<float>::max() ||
			najmanjimi != std::numeric_limits<float>::max()) {
			//preskocite taj izvor, on mi je skriven objektom kojemu sam upravo nasao sjeciste sa zrakom
			continue;
		}
		else {
			//izracunaj intenzitet kojim trenutni izvor(light) osvjetljava sjeciste pointWorld
			color += obojajPhong(k, pointWorld, light);
			//color.x += 0.05;
		}
	}
	
	if (dubinaRek < dubinaRekurzije + 1) {
		//glm::vec3 normalaNaKugli = glm::normalize(pointWorld - k->getCenter());
		//glm::vec3 reflectedBeamVector = glm::reflect(-r.getd(), normalaNaKugli);
		//color += /*k.krefglm::vec3(0.2f) */ slijedi(Zraka( pointWorld, glm::reflect(-r.getd(), reflectedBeamVector)) );
		glm::vec3 normalaNaTijelu = k->getNormalInPoint(pointWorld);
		glm::vec3 reflectedBeamVector = glm::reflect(+r.getd(), normalaNaTijelu);
		dubinaRek++;
		//std::cout << color.x << " " << color.y << " " << color.z << std::endl;
		glm::vec3 slijediZraku = slijedi(Zraka(pointWorld, reflectedBeamVector));
		if (k->front == true) {
			color += k->getKref() * slijediZraku;
		}
		else {
			color += k->getBKref() * slijediZraku;
		}
	}
	
	color = glm::clamp(color, 0.0f, 1.0f);
	return color;
}
glm::vec3 slijedi(Zraka r) {
	// izracunaj sjecista zrake sa svim objektima u sceni i pronadi najblize sjeciste
	float najmanjaLambda = std::numeric_limits<float>::max();
	float najmanjalamb = std::numeric_limits<float>::max();
	float najmanjimi = std::numeric_limits<float>::max();
	float minDistance = std::numeric_limits<float>::max();

	int br = 0;
	int closestObjectIndex = 99;
	for (auto& obj : objects) {
		if (obj->getID() == 1) {
			float lambda = obj->izracunajSjecisteZrakeiObjekta(r); // SAMO KUGLA
			if (lambda < najmanjaLambda) {
				/*najmanjaLambda = lambda;
				closestObjectIndex = br;*/

				float distance = glm::distance(r.getV(), r.getPointParametrized(lambda));
				if (distance < minDistance) {
					minDistance = distance;
					najmanjaLambda = lambda;
					closestObjectIndex = br;
				}
			}
		}
		if (obj->getID() == 2) {
			float lamb = std::numeric_limits<float>::max();
			float mi = std::numeric_limits<float>::max();
			glm::vec3 tockaNaRavnini = obj->izracunajSjecisteZrakeiObjekta(r, lamb, mi);
			
			// DAJ SANSU BILO KOJEMU KOJI IMA RAZUMNO RJESENJE ZA lamb i mi tj. ispitaj im distance
			if (lamb != std::numeric_limits<float>::max() || 
				mi   != std::numeric_limits<float>::max()) 
			{
				float distance = glm::distance(r.getV(), obj->getPointOnPlane(lamb,mi));
				if (distance < minDistance) {
					minDistance = distance;
					najmanjalamb = lamb; najmanjimi = mi;
					closestObjectIndex = br;
				}
				
			}
		}
		br++;
		//najmanjaLambda = std::min(najmanjaLambda, lambda);
	}

	

	//ako postoji sjeciste
	if (najmanjaLambda < std::numeric_limits<float>::max() ||
		najmanjalamb < std::numeric_limits<float>::max() ||
		najmanjimi < std::numeric_limits<float>::max()) {
		/*glColor3f(1, 1, 1);*/
		if (objects[closestObjectIndex]->getID() == 2) {
			glm::vec3 obrnutiSmjerZrake = -r.getd();
			glm::vec3 normala = objects[closestObjectIndex]->getNormal();
			if (glm::dot(obrnutiSmjerZrake, normala) > 0) {
				objects[closestObjectIndex]->front = true;
			}
			else {
				objects[closestObjectIndex]->front = false;
			}
			/*if (objects[closestObjectIndex]->front == false) {
				std::cout << "AAAAA" << std::endl;
			}*/
		}

		glm::vec3 utvrdenaBoja = utvrdiBoju(objects[closestObjectIndex], r, najmanjaLambda, najmanjalamb, najmanjimi);
		return utvrdenaBoja;
	}
	else {
		return ga; // pozadinska boja
	}
	return glm::vec3();
}
void postaviBoju(int x, int y, glm::vec3 slijediBoja){

	glColor4f(slijediBoja.x, slijediBoja.y, slijediBoja.z, 0.0f);
	glBegin(GL_POINTS);
	glVertex2i(x, y);
	glEnd();

}
void rayCast() {
	initObjects();

	glm::vec3 i, j;
	glm::vec3 GuRavnini;

	// dobij i i j vektore
	getNearPlaneInfo(i, j, GuRavnini);

	// ZA SVAKI PIKSEL EKRANA
	for (int x = 0; x < SW; x++) {
		for (int y = 0; y < SH; y++) {

			// IZRACUNAJ ZRAKU OD OKA DO SLIKOVNOG ELEMENTA (x,y)
			float clanUzI = -l + (x / SW) * (l + r);
			float clanUzJ = -b + (y / SH) * (t + b);
			glm::vec3 v = GuRavnini + i * glm::vec3(clanUzI) + j * glm::vec3(clanUzJ);

			// ZRAKA KOJA POLAZI IZ V I IMA SMJER (v-O) / ||v - O||
			Zraka r(v, glm::normalize(v - O));

			dubinaRek = 0;
			postaviBoju(x, y, slijedi(r));
		}
	}
}
void renderScene() {
	rayCast();

	/*glPushMatrix();
	glColor3f(1.0, 1.0, 1.0);

	kocka1();

	glm::vec3 centerSphere(1.0f, 1.0f, 0.0f);
	kugla(centerSphere, 5.0f);

	krpica();
	glPopMatrix();*/
}


void MainExercise::display() {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT /*| GL_DEPTH_BUFFER_BIT*/);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//gluLookAt(O.x, O.y, O.z, G.x, G.y, G.z, Vup.x, Vup.y, Vup.z); // transformacija pogleda


	renderScene();

	glutSwapBuffers();
}
void MainExercise::reshape(int width, int height) {
	SW = width; SH = height;
	glClear(GL_COLOR_BUFFER_BIT /*| GL_DEPTH_BUFFER_BIT*/);
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, SW-1, 0, SH-1);
	//glOrtho(-SW / 2, SW / 2, -SH / 2, SH / 2, 3, 100);
	//glOrtho(-5.19, 5.19, -5.19, 5.19, 3, 40); // transformacija projekcijska
	//glFrustum(-1.71f, 1.71f, -1.71f, 1.71f, 3.0f, 100.0f);
	//gluPerspective(60.0f, width / height, 3.0f, 100.0f);
	glMatrixMode(GL_MODELVIEW);
}

void updatePerspective() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, SW-1, 0, SH-1);
	//glOrtho(-SW / 2, SW / 2, -SH / 2, SH / 2, 3, 100);
	//glOrtho(-5.19, 5.19, -5.19, 5.19, 3, 40); // transformacija projekcijska
	//glFrustum(-1.71f, 1.71f, -1.71f, 1.71f, 3.0f, 100.0f);
	//gluPerspective(60.0f, SW / SH, 3.0f, 100.0f);
	glMatrixMode(GL_MODELVIEW);
}
void MainExercise::OnMouseClick(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {

	}

}
void MainExercise::OnKeyboardClick(unsigned char key, int x, int y) {
	if (key == 'q')
		O.x += 3.0f;
	if (key == 'w')
		O.y += 3.0f;
	if (key == 'e')
		O.z += 3.0f;
	if (key == 'a')
		O.x -= 3.0f;
	if (key == 's')
		O.y -= 3.0f;
	if (key == 'd')
		O.z -= 3.0f;
	updatePerspective();
	glutPostRedisplay();
}
void initGL() {
	std::cout << " aaaaa" << std::endl;
	//glEnable(GL_DEPTH_TEST);
}
void MainExercise::run(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(_windowWidth, _windowHeight);
	glutInitWindowPosition(168, 168);
	glutCreateWindow("Primjer Ray Tracing Algoritma");

	initGL();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);

	//glutMouseFunc(OnMouseClick);
	glutKeyboardFunc(OnKeyboardClick);
	glutMainLoop();
}