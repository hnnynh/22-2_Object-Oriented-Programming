////////////////////////////////////////////////////////////////////////////////
//
// File: virtualLego.cpp
//
// Original Author: 박창현 Chang-hyeon Park, 
// Modified by Bong-Soo Sohn and Dong-Jun Kim
// 
// Originally programmed for Virtual LEGO. 
// Modified later to program for Virtual Billiard.
//        
////////////////////////////////////////////////////////////////////////////////

#include "d3dUtility.h"
#include <vector>
#include <ctime>
#include <cstdlib>
#include <cstdio>
#include <cassert>

IDirect3DDevice9* Device = NULL;

// window size
const int Width  = 1024;
const int Height = 768;

// global values for game setting
const int brick_ball = 45;
bool start = false;	
int life = 5;
int point = 0;


// initialize the position (coordinate) of each ball
const float spherePos[brick_ball][2] = { {3.5f,-2.0f},{3.5f,-1.5f},{3.5f,-1.0f},{3.5f,-0.5f},{3.5f,0},{3.5f,0.5f},{3.5f,1.0f},{3.5f,1.5f},{3.5f,2.0f},				// 공 추가 및 위치 변경
										{-1.5f,-2.0f},{-1.5f,-1.5f},{-1.5f,-1.0f},{-1.5f,-0.5f},{-1.5f,0},{-1.5f,0.5f},{-1.5f,1.0f},{-1.5f,1.5f},{-1.5f,2.0f},
										{3.0f,-2.0f},{2.5f,-2.0f},{2.0f,-2.0f},{1.5f,-2.0f},{1.0f,-2.0f},{0.5f,-2.0f},{0,-2.0f},
										{-1.5f,-2.0f},{-1.0f,-2.0f},{-0.5f,-2.0f},{3.0f,2.0f},{2.5f,2.0f},{2.0f,2.0f},{1.5f,2.0f},
										{1.0f,2.0f},{0.5f,2.0f},{0,2.0f},{-1.5f,2.0f},{-1.0f,2.0f},{-0.5f,2.0f},
										{2.0f,1.0f},{2.0f,-1.0f},{0,1.0f},{0,0.5f},{0,0},{0,-0.5f},{0,-1.0f} };

// -----------------------------------------------------------------------------
// Transform matrices
// -----------------------------------------------------------------------------
D3DXMATRIX g_mWorld;
D3DXMATRIX g_mView;
D3DXMATRIX g_mProj;

#define M_RADIUS 0.21   // ball radius
#define PI 3.14159265
#define M_HEIGHT 0.01
#define DECREASE_RATE 0.9982

// -----------------------------------------------------------------------------
// CSphere class definition
// -----------------------------------------------------------------------------

class CSphere {
private :
	float					center_x, center_y, center_z;
    float                   m_radius;
	float					m_velocity_x;
	float					m_velocity_z;

	bool					isControlBall = false;

public:
    CSphere(void)
    {
        D3DXMatrixIdentity(&m_mLocal);
        ZeroMemory(&m_mtrl, sizeof(m_mtrl));
        m_radius = 0;
		m_velocity_x = 0;
		m_velocity_z = 0;
        m_pSphereMesh = NULL;
    }
    ~CSphere(void) {}

public:
    bool create(IDirect3DDevice9* pDevice, D3DXCOLOR color = d3d::WHITE)
    {
        if (NULL == pDevice)
            return false;
		
        m_mtrl.Ambient  = color;
        m_mtrl.Diffuse  = color;
        m_mtrl.Specular = color;
        m_mtrl.Emissive = d3d::BLACK;
        m_mtrl.Power    = 5.0f;
		
        if (FAILED(D3DXCreateSphere(pDevice, getRadius(), 50, 50, &m_pSphereMesh, NULL)))
            return false;
        return true;
    }
	
    void destroy(void)
    {
        if (m_pSphereMesh != NULL) {
            m_pSphereMesh->Release();
            m_pSphereMesh = NULL;
        }
    }

    void draw(IDirect3DDevice9* pDevice, const D3DXMATRIX& mWorld)
    {
        if (NULL == pDevice)
            return;
        pDevice->SetTransform(D3DTS_WORLD, &mWorld);
        pDevice->MultiplyTransform(D3DTS_WORLD, &m_mLocal);
        pDevice->SetMaterial(&m_mtrl);
		m_pSphereMesh->DrawSubset(0);
    }
	
    bool hasIntersected(CSphere& ball) 
	{	
		
		D3DXVECTOR3 diff = this->getCenter() - ball.getCenter();

		float distance;
		distance = sqrt(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z);

		if (distance <= this->getRadius() + ball.getRadius())
			return true;
	
		return false;
	}
	
	void hitBy(CSphere& ball)					// 공끼리 부딪히면
	{	
		if (hasIntersected(ball)) {

			if (ball.getIsControlBall()) {
				if (this->getVelocity_Z() > 0) 					// 무조건 위로 올라가는 방향으로 설정
					this->setPower(this->getVelocity_Z(), -this->getVelocity_X());
				else 
					this->setPower(-this->getVelocity_Z(), this->getVelocity_X());	
				return;
			}

			if (this->getVelocity_Z() > 0) 					// 무조건 아래로 내려오는 방향으로 설정
				this->setPower(-this->getVelocity_Z(), this->getVelocity_X());
			else 
				this->setPower(this->getVelocity_Z(), -this->getVelocity_X());
			
			ball.setCenter(-5.0f, -5.0f, -5.0f);
			point++;

			if (point == brick_ball) {
				ball.setCenter(-5.0f, -5.0f, -5.0f);
				::MessageBox(0, "YOU WIN !!", "YOU WIN", 0);
				exit(0);
			}
		}
	}

	void ballUpdate(float timeDiff) 
	{
		const float TIME_SCALE = 3.3;
		D3DXVECTOR3 cord = this->getCenter();
		double vx = abs(this->getVelocity_X());
		double vz = abs(this->getVelocity_Z());

		if(vx > 0.01 || vz > 0.01)
		{
			float tX = cord.x + TIME_SCALE*timeDiff*m_velocity_x;
			float tZ = cord.z + TIME_SCALE*timeDiff*m_velocity_z;

			//correction of position of ball
			// Please uncomment this part because this correction of ball position is necessary when a ball collides with a wall
			/*if(tX >= (4.5 - M_RADIUS))
				tX = 4.5 - M_RADIUS;
			else if(tX <=(-4.5 + M_RADIUS))
				tX = -4.5 + M_RADIUS;
			else if(tZ <= (-3 + M_RADIUS))
				tZ = -3 + M_RADIUS;
			else if(tZ >= (3 - M_RADIUS))
				tZ = 3 - M_RADIUS;*/
			
			this->setCenter(tX, cord.y, tZ);
		}
		else { this->setPower(0,0);}
		//this->setPower(this->getVelocity_X() * DECREASE_RATE, this->getVelocity_Z() * DECREASE_RATE);
	}

	double getVelocity_X() { return this->m_velocity_x;	}
	double getVelocity_Z() { return this->m_velocity_z; }

	void setPower(double vx, double vz)
	{
		this->m_velocity_x = vx;
		this->m_velocity_z = vz;
	}

	void setCenter(float x, float y, float z)
	{
		D3DXMATRIX m;
		center_x=x;	center_y=y;	center_z=z;
		D3DXMatrixTranslation(&m, x, y, z);
		setLocalTransform(m);
	}
	
	float getRadius(void)  const { return (float)(M_RADIUS);  }
    const D3DXMATRIX& getLocalTransform(void) const { return m_mLocal; }
    void setLocalTransform(const D3DXMATRIX& mLocal) { m_mLocal = mLocal; }
    D3DXVECTOR3 getCenter(void) const
    {
        D3DXVECTOR3 org(center_x, center_y, center_z);
        return org;
    }

	void setIsControlBall(void) {
		this->isControlBall = true;
	}

	bool getIsControlBall(void) {
		return isControlBall;
	}
	
private:
    D3DXMATRIX              m_mLocal;
    D3DMATERIAL9            m_mtrl;
    ID3DXMesh*              m_pSphereMesh;
	
};



// -----------------------------------------------------------------------------
// CWall class definition
// -----------------------------------------------------------------------------

class CWall {

private:
	
    float					m_x;
	float					m_z;
	float                   m_width;
    float                   m_depth;
	float					m_height;
	
public:
    CWall(void)
    {
        D3DXMatrixIdentity(&m_mLocal);
        ZeroMemory(&m_mtrl, sizeof(m_mtrl));
        m_width = 0;
        m_depth = 0;
        m_pBoundMesh = NULL;
    }
    ~CWall(void) {}
public:
    bool create(IDirect3DDevice9* pDevice, float ix, float iz, float iwidth, float iheight, float idepth, D3DXCOLOR color = d3d::WHITE)
    {
        if (NULL == pDevice)
            return false;
		
        m_mtrl.Ambient  = color;
        m_mtrl.Diffuse  = color;
        m_mtrl.Specular = color;
        m_mtrl.Emissive = d3d::BLACK;
        m_mtrl.Power    = 5.0f;
		
        m_width = iwidth;
        m_depth = idepth;
		
        if (FAILED(D3DXCreateBox(pDevice, iwidth, iheight, idepth, &m_pBoundMesh, NULL)))
            return false;
        return true;
    }
    void destroy(void)
    {
        if (m_pBoundMesh != NULL) {
            m_pBoundMesh->Release();
            m_pBoundMesh = NULL;
        }
    }
    void draw(IDirect3DDevice9* pDevice, const D3DXMATRIX& mWorld)
    {
        if (NULL == pDevice)
            return;
        pDevice->SetTransform(D3DTS_WORLD, &mWorld);
        pDevice->MultiplyTransform(D3DTS_WORLD, &m_mLocal);
        pDevice->SetMaterial(&m_mtrl);
		m_pBoundMesh->DrawSubset(0);
    }
	
	bool hasIntersected(CSphere& ball) 
	{	
		float ball_x = ball.getCenter().x;
		float ball_z = ball.getCenter().z;

		float hit_x1 = this->m_x - this->m_width / 2 - ball.getRadius();
		float hit_x2 = this->m_x + this->m_width / 2 + ball.getRadius();
		float hit_z1 = this->m_z - this->m_depth / 2 - ball.getRadius();
		float hit_z2 = this->m_z + this->m_depth / 2 + ball.getRadius();

		if (hit_x1 <= ball_x && ball_x <= hit_x2 && hit_z1 <= ball_z && ball_z <= hit_z2)
			return true;

		return false;
	}

	void hitBy(CSphere& ball)						// 벽이랑 공이랑 부딪혔을 때 
	{
		if (hasIntersected(ball)) {
			
			float ball_x = ball.getCenter().x;
			float ball_z = ball.getCenter().z;

			float x1 = this->m_x - this->m_width / 2;
			float x2 = this->m_x + this->m_width / 2;
			float z1 = this->m_z - this->m_depth / 2;
			float z2 = this->m_z + this->m_depth / 2;
			
			// x 또는 z에 - 붙이는 것 결정
			if ((x1 <= ball_x && ball_x <= x2) && !(z1 <= ball_z && ball_z <= z2)) 
				ball.setPower(ball.getVelocity_X(), -ball.getVelocity_Z());
			else if(!(x1 <= ball_x && ball_x <= x2) && (z1 <= ball_z && ball_z <= z2)) 
				ball.setPower(-ball.getVelocity_X(), ball.getVelocity_Z());
		}
	}    
	
	void setPosition(float x, float y, float z)
	{
		D3DXMATRIX m;
		this->m_x = x;
		this->m_z = z;

		D3DXMatrixTranslation(&m, x, y, z);
		setLocalTransform(m);
	}
	
    float getHeight(void) const { return M_HEIGHT; }
	// 추가
	float getWidth(void) const { return this->m_width; }
	float getDepth(void) const { return this->m_depth; }

	D3DXVECTOR3 getCenter(void) const
	{
		D3DXVECTOR3 org(m_x, 0, m_z);
		return org;
	}
	
private :
    void setLocalTransform(const D3DXMATRIX& mLocal) { m_mLocal = mLocal; }
	
	D3DXMATRIX              m_mLocal;
    D3DMATERIAL9            m_mtrl;
    ID3DXMesh*              m_pBoundMesh;
};

// -----------------------------------------------------------------------------
// CLight class definition
// -----------------------------------------------------------------------------

class CLight {
public:
    CLight(void)
    {
        static DWORD i = 0;
        m_index = i++;
        D3DXMatrixIdentity(&m_mLocal);
        ::ZeroMemory(&m_lit, sizeof(m_lit));
        m_pMesh = NULL;
        m_bound._center = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
        m_bound._radius = 0.0f;
    }
    ~CLight(void) {}
public:
    bool create(IDirect3DDevice9* pDevice, const D3DLIGHT9& lit, float radius = 0.1f)
    {
        if (NULL == pDevice)
            return false;
        if (FAILED(D3DXCreateSphere(pDevice, radius, 10, 10, &m_pMesh, NULL)))
            return false;
		
        m_bound._center = lit.Position;
        m_bound._radius = radius;
		
        m_lit.Type          = lit.Type;
        m_lit.Diffuse       = lit.Diffuse;
        m_lit.Specular      = lit.Specular;
        m_lit.Ambient       = lit.Ambient;
        m_lit.Position      = lit.Position;
        m_lit.Direction     = lit.Direction;
        m_lit.Range         = lit.Range;
        m_lit.Falloff       = lit.Falloff;
        m_lit.Attenuation0  = lit.Attenuation0;
        m_lit.Attenuation1  = lit.Attenuation1;
        m_lit.Attenuation2  = lit.Attenuation2;
        m_lit.Theta         = lit.Theta;
        m_lit.Phi           = lit.Phi;
        return true;
    }
    void destroy(void)
    {
        if (m_pMesh != NULL) {
            m_pMesh->Release();
            m_pMesh = NULL;
        }
    }
    bool setLight(IDirect3DDevice9* pDevice, const D3DXMATRIX& mWorld)
    {
        if (NULL == pDevice)
            return false;
		
        D3DXVECTOR3 pos(m_bound._center);
        D3DXVec3TransformCoord(&pos, &pos, &m_mLocal);
        D3DXVec3TransformCoord(&pos, &pos, &mWorld);
        m_lit.Position = pos;
		
        pDevice->SetLight(m_index, &m_lit);
        pDevice->LightEnable(m_index, TRUE);
        return true;
    }

    void draw(IDirect3DDevice9* pDevice)
    {
        if (NULL == pDevice)
            return;
        D3DXMATRIX m;
        D3DXMatrixTranslation(&m, m_lit.Position.x, m_lit.Position.y, m_lit.Position.z);
        pDevice->SetTransform(D3DTS_WORLD, &m);
        pDevice->SetMaterial(&d3d::WHITE_MTRL);
        m_pMesh->DrawSubset(0);
    }

    D3DXVECTOR3 getPosition(void) const { return D3DXVECTOR3(m_lit.Position); }

private:
    DWORD               m_index;
    D3DXMATRIX          m_mLocal;
    D3DLIGHT9           m_lit;
    ID3DXMesh*          m_pMesh;
    d3d::BoundingSphere m_bound;
};


// -----------------------------------------------------------------------------
// Global variables
// -----------------------------------------------------------------------------
CWall	g_legoPlane;
CWall	g_legowall[4];
CSphere	g_sphere[brick_ball];
CSphere	g_target_redball;
CSphere g_control_ball;
CLight	g_light;

double g_camera_pos[3] = {0.0, 5.0, -8.0};

// -----------------------------------------------------------------------------
// Functions
// -----------------------------------------------------------------------------


void destroyAllLegoBlock(void)
{
}

// initialization
bool Setup()
{
	int i;
	
    D3DXMatrixIdentity(&g_mWorld);
    D3DXMatrixIdentity(&g_mView);
    D3DXMatrixIdentity(&g_mProj);
		
	// create plane and set the position
    // if (false == g_legoPlane.create(Device, -1, -1, 9, 0.03f, 6, d3d::GREEN)) return false;
	if (false == g_legoPlane.create(Device, -1, -1, 9, 0.03f, 6, d3d::GREEN)) return false;

	g_legoPlane.setPosition(0.0f, -0.0006f / 5, 0.0f);
	
	// create walls and set the position. note that there are four walls
	if (false == g_legowall[0].create(Device, -1, -1, 9, 0.3f, 0.12f, d3d::DARKRED)) return false;
	g_legowall[0].setPosition(0.0f, 0.12f, 3.06f);
	if (false == g_legowall[1].create(Device, -1, -1, 9, 0.3f, 0.12f, d3d::DARKRED)) return false;
	g_legowall[1].setPosition(0.0f, 0.12f, -3.06f);
	if (false == g_legowall[2].create(Device, -1, -1, 0.12f, 0.3f, 6.24f, d3d::DARKRED)) return false;
	g_legowall[2].setPosition(4.56f, 0.12f, 0.0f);
	if (false == g_legowall[3].create(Device, -1, -1, 0, 0, 0, d3d::DARKRED)) return false;				// ball 움직이는 면 높이, 너비 등 0으로 설정
	g_legowall[3].setPosition(-4.56f, 0.12f, 0.0f);


	for (i=0;i<brick_ball;i++) {																					// ball 개수 추가, 위치 선정
		if (false == g_sphere[i].create(Device, d3d::YELLOW)) return false;								
		g_sphere[i].setCenter(spherePos[i][0], (float)M_RADIUS , spherePos[i][1]);
		g_sphere[i].setPower(0,0);
	}

    if (false == g_target_redball.create(Device, d3d::RED)) return false;
	g_target_redball.setCenter(-4.3f + 2 * (float)M_RADIUS, (float)M_RADIUS, 0);
	g_target_redball.setPower(0, 0);

	if (false == g_control_ball.create(Device, d3d::WHITE)) return false;
	g_control_ball.setCenter(-4.3f, (float)M_RADIUS, .0f);
	g_control_ball.setIsControlBall();
	g_control_ball.setPower(0, 0);

	// light setting 
    D3DLIGHT9 lit;
    ::ZeroMemory(&lit, sizeof(lit));
    lit.Type         = D3DLIGHT_POINT;
    lit.Diffuse      = d3d::WHITE; 
	lit.Specular     = d3d::WHITE * 0.9f;
    lit.Ambient      = d3d::WHITE * 0.9f;
    lit.Position     = D3DXVECTOR3(0.0f, 3.0f, 0.0f);
    lit.Range        = 100.0f;
    lit.Attenuation0 = 0.0f;
    lit.Attenuation1 = 0.9f;
    lit.Attenuation2 = 0.0f;
    if (false == g_light.create(Device, lit))
        return false;
	
	// Position and aim the camera.
	D3DXVECTOR3 pos(-8.0f, 12.0f, 0.0f);
	D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 up(0.0f, 2.0f, 0.0f);
	D3DXMatrixLookAtLH(&g_mView, &pos, &target, &up);
	Device->SetTransform(D3DTS_VIEW, &g_mView);
	
	// Set the projection matrix.
	D3DXMatrixPerspectiveFovLH(&g_mProj, D3DX_PI / 4,
        (float)Width / (float)Height, 1.0f, 100.0f);
	Device->SetTransform(D3DTS_PROJECTION, &g_mProj);
	
    // Set render states.
    Device->SetRenderState(D3DRS_LIGHTING, TRUE);
    Device->SetRenderState(D3DRS_SPECULARENABLE, TRUE);
    Device->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
	
	g_light.setLight(Device, g_mWorld);
	return true;
}

void Cleanup(void)
{
    g_legoPlane.destroy();
	for(int i = 0 ; i < 4; i++) {
		g_legowall[i].destroy();
	}
    destroyAllLegoBlock();
    g_light.destroy();
}


// timeDelta represents the time between the current image frame and the last image frame.
// the distance of moving balls should be "velocity * timeDelta"
bool Display(float timeDelta)
{
	int i = 0;
	int j = 0;

	Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
	Device->BeginScene();

	RECT rect = { 100, 100, Width, Height };

	if (Device)
	{
		Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00afafaf, 1.0f, 0);
		Device->BeginScene();

		// update the position of each ball. during update, check whether each ball hit by walls.

		for (i = 0; i < brick_ball; i++) {
			g_sphere[i].ballUpdate(timeDelta);
		}
		g_target_redball.ballUpdate(timeDelta);
		for (i = 0; i < 4; i++) { g_legowall[i].hitBy(g_target_redball); }
		g_control_ball.ballUpdate(timeDelta);
		g_target_redball.hitBy(g_control_ball);

		// check whether any two balls hit together and update the direction of balls
		for (i = 0; i < brick_ball; i++) {
			g_target_redball.hitBy(g_sphere[i]);
		}

		if (g_target_redball.getCenter().x < -4.8f + (float)M_RADIUS) {
			start = false;
			life--;
			if (life == 0) {
				::MessageBox(0, "GAME OVER !!", "GAME OVER", 0);
				exit(0);
			}
			g_target_redball.setCenter(-4.3f + 2 * (float)M_RADIUS, (float)M_RADIUS, 0);
			g_target_redball.setPower(0, 0);
		}
		

		// draw plane, walls, and spheres
		g_legoPlane.draw(Device, g_mWorld);
		for (i=0;i<4;i++) 	{
			g_legowall[i].draw(Device, g_mWorld);
		}
		for (i = 0; i < brick_ball; i++) {
			g_sphere[i].draw(Device, g_mWorld);
		}
		g_target_redball.draw(Device, g_mWorld);
		g_control_ball.draw(Device, g_mWorld);
        g_light.draw(Device);
		
		Device->EndScene();
		Device->Present(0, 0, 0, 0);
		Device->SetTexture( 0, NULL );
	}
	return true;
}

LRESULT CALLBACK d3d::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static bool wire = false;
	static int old_z = 0;
    static enum { WORLD_MOVE, LIGHT_MOVE, BLOCK_MOVE } move = WORLD_MOVE;
	
	switch( msg ) {
	case WM_DESTROY:
        {
			::PostQuitMessage(0);
			break;
        }
	case WM_KEYDOWN:
        {
            switch (wParam) {
            case VK_ESCAPE:
				::DestroyWindow(hwnd);
                break;
            case VK_RETURN:
                if (NULL != Device) {
                    wire = !wire;
                    Device->SetRenderState(D3DRS_FILLMODE,
                        (wire ? D3DFILL_WIREFRAME : D3DFILL_SOLID));
                }
                break;
            case VK_SPACE:

				// 시작 속도 설정
				if (!start) {
					start = true;
					if(life % 2)
						g_target_redball.setPower(2.0f, 1.0f);
					else
						g_target_redball.setPower(1.0f, -2.0f);
				}
				break;

			}
			break;
        }
		
	case WM_MOUSEMOVE:
        {
            int new_z = LOWORD(lParam);				// z축 양옆으로만 이동
			float dz;
			float z;

			// 선상에서 + 양쪽 벽 안에서 움직이도록 설정
			float wall1 = g_legowall[0].getCenter().z - g_legowall[0].getDepth() / 2 - g_control_ball.getRadius();
			float wall2 = g_legowall[1].getCenter().z + g_legowall[0].getDepth() / 2 + g_control_ball.getRadius();

			if (LOWORD(wParam) & MK_LBUTTON) {				// 왼쪽 마우스로 control ball 움직이기
				
				dz = (old_z - new_z);

				D3DXVECTOR3 coord3d = g_control_ball.getCenter();
				g_control_ball.setCenter(-4.3f, (float)M_RADIUS, .0f);
			
				z = coord3d.z + dz * 0.01f;

				if (z > wall1)
					z = wall1;
				else if (z < wall2)
					z = wall2;

				g_control_ball.setCenter(-4.3f, (float)M_RADIUS, z);		
			}
		
			old_z = new_z;
			move = WORLD_MOVE;
	
            break;
        }
	}
	
	return ::DefWindowProc(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hinstance,
				   HINSTANCE prevInstance, 
				   PSTR cmdLine,
				   int showCmd)
{
    srand(static_cast<unsigned int>(time(NULL)));
	
	if(!d3d::InitD3D(hinstance,
		Width, Height, true, D3DDEVTYPE_HAL, &Device))
	{
		::MessageBox(0, "InitD3D() - FAILED", 0, 0);
		return 0;
	}
	
	if(!Setup())
	{
		::MessageBox(0, "Setup() - FAILED", 0, 0);
		return 0;
	}
	
	d3d::EnterMsgLoop( Display );
	
	Cleanup();
	
	Device->Release();
	
	return 0;
}