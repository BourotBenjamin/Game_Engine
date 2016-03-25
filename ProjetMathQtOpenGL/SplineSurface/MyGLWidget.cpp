#define GLEW_STATIC 1
#include <GL\glew.h>
#include "MyGLWidget.h"
#include <QtGui\qevent.h>



void MyGLWidget::initializeGL()
{
	glewInit();
	setMinimumSize(800, 800);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);

	setMouseTracking(true);
	projection.Perspective(45.f, width(), height(), 0.1f, 1000.f);
	cam = Camera(Point(0, 0, -7.0f), Point(0,0,0), Point(0, 1, 0));

	for (int i = 0; i < 50; ++i)
		buffKey[1] = 0;

	xRel = 0;
	yRel = 0;

	shad = std::unique_ptr<Shader>(new Shader("VertexObj.vs", "FragmentObj.fs", "", ""));
	/*
	skyboxShader = std::unique_ptr<Shader>(new Shader("skybox.vs", "skybox.fs", "", ""));
	normalMap = std::unique_ptr<Shader>(new Shader("normalMap.vs", "normalMap.fs", "", ""));
	shadowDepth = std::unique_ptr<Shader>(new Shader("depth.vs", "depth.fs", "", ""));
	lampshad = std::unique_ptr<Shader>(new Shader("lamp.vs", "lamp.fs", "", ""));
	updateParticules = std::unique_ptr<EsgiShader>(new EsgiShader());//"ps_update.vs", "ps_update.fs", "ps_update.gs", ""));
	billboard = std::unique_ptr<Shader>(new Shader("billboard.vs", "billboard.fs", "billboard.gs", ""));
	*/
	rightClick = false;
	modelView.identity();

	cam.setPosition(Point(0, 20.f, -10));
	cam.setPointcible(Point());
	cam.lookAt(modelView);

	glGenBuffers(1, &UBO);
	glBindBuffer(GL_UNIFORM_BUFFER, UBO);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(float)* 16 * 2 + sizeof(float)* 3, nullptr, GL_STREAM_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, UBO);
	//glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glGenBuffers(1, &lightUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, lightUBO);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(float)* 16 * 3 + sizeof(float)* 3, nullptr, GL_STREAM_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, lightUBO);

	auto program = shad->getProgramID();
	glUseProgram(program);
	auto blockIndex = glGetUniformBlockIndex(program, "ViewProj");
	glUniformBlockBinding(program, blockIndex, 0);
	glUseProgram(0);
	/*
	program = normalMap->getProgramID();
	glUseProgram(program);
	blockIndex = glGetUniformBlockIndex(program, "ViewProj");
	glUniformBlockBinding(program, blockIndex, 0);
	blockIndex = glGetUniformBlockIndex(program, "LightProj");
	glUniformBlockBinding(program, blockIndex, 1);
	glUseProgram(0);

	program = skyboxShader->getProgramID();
	glUseProgram(program);
	blockIndex = glGetUniformBlockIndex(program, "ViewProj");
	glUniformBlockBinding(program, blockIndex, 0);
	glUseProgram(0);

	program = shadowDepth->getProgramID();
	glUseProgram(program);
	blockIndex = glGetUniformBlockIndex(program, "ViewProj");
	glUniformBlockBinding(program, blockIndex, 0);
	blockIndex = glGetUniformBlockIndex(program, "LightProj");
	glUniformBlockBinding(program, blockIndex, 1);
	glUseProgram(0);

	program = lampshad->getProgramID();
	glUseProgram(program);
	blockIndex = glGetUniformBlockIndex(program, "ViewProj");
	glUniformBlockBinding(program, blockIndex, 0);
	blockIndex = glGetUniformBlockIndex(program, "LightProj");
	glUniformBlockBinding(program, blockIndex, 1);
	glUseProgram(0);

	program = billboard->getProgramID();
	glUseProgram(program);
	blockIndex = glGetUniformBlockIndex(program, "ViewProj");
	glUniformBlockBinding(program, blockIndex, 0);
	glUseProgram(0);

	// Configure depth map FBO
    glGenFramebuffers(1, &shadowMapFBO);
    // - Create depth texture
    glGenTextures(1, &shadowMapTex);
    glBindTexture(GL_TEXTURE_2D, shadowMapTex);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_CLAMP_TO_BORDER);
    GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMapTex, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	cat = std::unique_ptr<Objet>(new Objet("cat.obj"));
	cyborg = std::unique_ptr<Objet>(new Objet("cyborg.obj"));

	floor = std::unique_ptr<Objet>(new Objet("cube.obj"));

	wall1 = std::unique_ptr<Objet>(new Objet("wall.obj"));
	wall2 = std::unique_ptr<Objet>(new Objet("wall.obj"));
	wall3 = std::unique_ptr<Objet>(new Objet("wall.obj"));
	wall4 = std::unique_ptr<Objet>(new Objet("wall.obj"));

	float wallY = 1;
	float floorDepth = 2.f;

	wall1->position = Point(15, wallY, 0);
	wall1->scale = Point(floorDepth, 15.f, 30.f);

	wall2->position = Point(-15, wallY, 0);
	wall2->scale = Point(floorDepth, 15.f, 30.f);

	wall3->position = Point(0, wallY, 15);
	wall3->scale = Point(30.f, 15.f, floorDepth);

	wall4->position = Point(0, wallY, -15);
	wall4->scale = Point(30.f, 15.f, floorDepth);

	floor->setPost(Point(0.f, -floorDepth/2-0.15, 0.f));
	floor->setScale(Point(30.f, floorDepth, 30.f));
	cat->setPost(Point(-2.5f, 0, 0));
	cat->setScale(Point(4.f, 4.f, 4.f));
	cyborg->setPost(Point(0, 0, 3));



	std::vector<std::string> v = {
		"skybox/right.jpg",
		"skybox/left.jpg",
		"skybox/top.jpg",
		"skybox/bottom.jpg",
		"skybox/back.jpg",
		"skybox/front.jpg",
	};
	 
	skybox = std::unique_ptr<Objet>(new Objet(v));

	lamp = std::unique_ptr<Objet>(new Objet("lamp.obj"));

	lightCoord = Mat4x4(0);
	lightCoord.identity();
	lightCoord *= Mat4x4::translate(0.5f, 0.5f, 0.5f);
	lightCoord *= Mat4x4::scale(0.5f, 0.5f, 0.5f);

	lightPos = Point(0.f, 15.f, -25.f);
	lamp->position = lightPos;

	path = std::unique_ptr<Bezier>(new Bezier(3000.f));
	path->pushPoint(Point(0, 0, 0));
	path->pushPoint(Point(-2.f, 0.0f, 3.f));
	path->pushPoint(Point(1.f, 0.0f, 6.f));
	path->pushPoint(Point(2, 0.0f, 1));
	path->pushPoint(Point(4, 0, -4));

	path->computePointInCourbe();

	auto listeSpline = path->getPointInCourbe();
	indexInSpline = 1;
	down = false;


	particuleEmitter = std::unique_ptr<Objet>(new Objet("wall.obj", 100, *updateParticules, "ps_update.vs", "ps_update.fs", "ps_update.gs"));
	*/

	initScene(program);

	timer.Begin();
	
}

void MyGLWidget::initScene(GLuint program)
{
	memoryManager = MemoryManager();
	threadPool = new ThreadPool(std::thread::hardware_concurrency());
	std::string file("rock.obj");
	AllocatorVector<World> worlds(memoryManager, 1);
	w = worlds.allocation();
	AllocatorVector<int> maps(memoryManager, w->ZONES_X * w->ZONES_Y + w->NB_MAX_OBJ * 10);
	w->setMaps(maps.allocation());
	AllocatorVector<Transform> transforms(memoryManager, 100);
	AllocatorVector<RigidBody> rigidBodies(memoryManager, 100);
	AllocatorVector<SphereCollider> colliders(memoryManager, 100);
	AllocatorVector<Mesh> meshs(memoryManager, 100);
	AllocatorVector<MeshRenderer> meshRenderers(memoryManager, 100);
	AllocatorVector<GameObject> gameObjects(memoryManager, 100);
	for (int i = 0; i < 100; i++)
	{
		Transform* tr = transforms.allocation(*w, rand() % 2000, rand() % 2000, rand() % 2000);
		RigidBody* rb = rigidBodies.allocation(*tr);
		SphereCollider* col = colliders.allocation(*tr, *rb, *w);
		Mesh* m = meshs.allocation(program, file);
		MeshRenderer* r = meshRenderers.allocation(*tr, *m);
		GameObject* go = gameObjects.allocation(tr, r, rb, (Collider*) col);
	}
	firstRigidBody = rigidBodies.getFirst();
	firstCollider = (Collider*) colliders.getFirst();
	firstMeshRenderer = meshRenderers.getFirst();
}

void MyGLWidget::updateWidget(float deltaTime)
{
	// stock first
	threadPool->assign<RigidBody>(firstRigidBody,100, RigidBody::updateAll);
	threadPool->assign<Collider>(firstCollider, 100, Collider::updateAll);
	/*
	cam.deplacer(buffKey, xRel, yRel, deltaTime);
	xRel = 0;
	yRel = 0;

	auto listeSpline = path->getPointInCourbe();
	Point p;
	if (!down)
	{
		p = (listeSpline[indexInSpline] - listeSpline[indexInSpline - 1]);
		p.normalize();
		cat->position += p * deltaTime* 5.f;
		++indexInSpline;
	}
	else
	{
		p = (listeSpline[indexInSpline - 1] - listeSpline[indexInSpline]);
		p.normalize();
		cat->position += p *deltaTime * 5.f;
		--indexInSpline;
	}
	auto q = Quaternion::RotationBetweenVectors(cat->lookat, p);
	cat->lookat = p;
	cat->rotation += q.toEulerAngle();
	if (indexInSpline == listeSpline.size() - 1 && down == false)
		down = true;
	else if (indexInSpline == 1 && down == true)
		down = false;


	auto t = timer.GetElapsedTimeSince(timer.GetStartTime())/2;
	//auto lampMove = Point(std::cos(t), 0, std::sin(t));
	auto lampMove = Point(std::cos(t), 0, 0);
	lampMove.normalize();
	lightPos += lampMove * deltaTime * 10.f;
	lamp->position = lightPos;

	cyborg->rotation = Point(0, std::cosf(t), 0);

	//lamp->setLookAtPoint(lampMove);
	
	auto program = updateParticules->GetProgram();
	glUseProgram(program);
	particuleEmitter->UpdateParticules(program, deltaTime * 1000.f);



	cam.lookAt(modelView);*/
}



void MyGLWidget::paintGL()
{	
	/*
	glViewport(0, 0, width(), height());
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	projection.Perspective(45.f, width(), height(), 0.1f, 1000.f);
	GLfloat near_plane = 0.1f, far_plane = 70.f;
	float spotDim = 50.f;
	lightProjection.Orthographic(-spotDim, spotDim, -spotDim, spotDim, near_plane, far_plane);
	//lightProjection.Perspective(45.0f, (GLfloat)SHADOW_WIDTH, (GLfloat)SHADOW_HEIGHT, near_plane, far_plane);
	lightView.lookAt(lightPos,floor->position - lightPos, Point(0.f, 1.f, 0.f));

	auto p = cam.getPos();
	float pos[3];
	pos[0] = p.x;
	pos[1] = p.y;
	pos[2] = p.z;

	float lpos[3];
	lpos[0] = lightPos.x;
	lpos[1] = lightPos.y;
	lpos[2] = lightPos.z;

	glBindBuffer(GL_UNIFORM_BUFFER, UBO);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float) * 16, modelView.getMatrix());
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float)* 16, sizeof(float)* 16, projection.getMatrix());
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float)* 16 * 2, sizeof(float)* 3, pos);

	glBindBuffer(GL_UNIFORM_BUFFER, lightUBO);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float) * 16, lightView.getMatrix());
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float)* 16, sizeof(float)* 16, lightProjection.getMatrix());
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float)* 16*2, sizeof(float)* 16, lightCoord.getMatrix());
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float)* 16 * 3, sizeof(float)* 3, lpos);
	
    // - now render scene from light's point of view
	auto program = shadowDepth->getProgramID();
	glUseProgram(program);
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
		glEnable(GL_CULL_FACE);
		glFrontFace(GL_CW);
		//particuleEmitter->renderBillBoardParticules(program);
        renderScene(program);
		glDisable(GL_CULL_FACE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
	*/

	// 2. Render scene as normal
    glViewport(0, 0, width(), height());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glEnable(GL_POLYGON_OFFSET_FILL);
	//glPolygonOffset(1.0, 2.0);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	/*glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);*/

	threadPool->assign<MeshRenderer>(firstMeshRenderer, 100, MeshRenderer::renderAll);
	/*
	program = billboard->getProgramID();
	glUseProgram(program);
	//particuleEmitter->renderBillBoardParticules(program);

	//particuleEmitter->currVB = particuleEmitter->currTFB;
	//particuleEmitter->currTFB = (particuleEmitter->currTFB + 1) & 0x1;
	std::swap(particuleEmitter->currVB, particuleEmitter->currTFB);
	particuleEmitter->currSource ^= 1;
	
	program = normalMap->getProgramID();
	glUseProgram(program);
	renderScene(program, shadowMapTex);

	program = lampshad->getProgramID();
	glUseProgram(program);
	lamp->render(program);
	
	program = skyboxShader->getProgramID();
	glUseProgram(program);
	skybox->renderCubeMap(program);

	*/
	//timer.End();
	//std::cout << timer.GetElapsedTime() << "\n";
}


void MyGLWidget::RenderQuad()
{
	GLuint quadVAO = 0;
	GLuint quadVBO;
	if (quadVAO == 0)
	{
		GLfloat quadVertices[] = {
			// Positions        // Texture Coords
			-1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
			1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// Setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

void MyGLWidget::renderScene(GLuint& program, GLuint shadowTex)
{

	//cat->render(program, shadowTex);
	//cyborg->render(program, shadowTex);
	cat->render(program, shadowTex);
	cyborg->render(program, shadowTex);
	floor->render(program, shadowTex);
	//wall1->render(program, shadowTex);
	wall2->render(program, shadowTex);
	//wall3->render(program, shadowTex);
	wall4->render(program, shadowTex);
	
	
}

void MyGLWidget::mousePressEvent(QMouseEvent * e)
{
	if (e->button() == Qt::MouseButton::RightButton)
		rightClick = true;
}

void MyGLWidget::mouseMoveEvent(QMouseEvent * e)
{
	if (rightClick)
	{
		xRel = e->x() - m_oldMousePos.Getx();
		yRel = e->y() - m_oldMousePos.Gety();
	}
	m_oldMousePos = Point(e->x(), e->y(), 0);
}

void MyGLWidget::mouseReleaseEvent(QMouseEvent * e)
{
	if (e->button() == Qt::MouseButton::RightButton)
		rightClick = false;
}

void MyGLWidget::resizeEvent(QResizeEvent *e)
{
	projection.Perspective(45.0f, width(),height(), 0.1f,1000.0f );
}

bool MyGLWidget::event(QEvent *e)
{
	if (e->type() == QEvent::KeyPress) {
		QKeyEvent *ke = static_cast<QKeyEvent *>(e);
		if (ke->key() == Qt::Key_Left) 
			buffKey[2] = 1;
		
		if (ke->key() == Qt::Key_Right) 
			buffKey[3] = 1;
		
		if (ke->key() == Qt::Key_Up) 
			buffKey[0] = 1;
		
		if (ke->key() == Qt::Key_Down) 
			buffKey[1] = 1;
		
		if (ke->key() == Qt::Key_Space)
			buffKey[4] = 1;

		if (ke->key() == Qt::Key_Q)
			buffKey[2] = 1;
		
		if (ke->key() == Qt::Key_D) 
			buffKey[3] = 1;
		
		if (ke->key() == Qt::Key_Z) 
			buffKey[0] = 1;
		
		if (ke->key() == Qt::Key_S) 
			buffKey[1] = 1;
		
		if (ke->key() == Qt::Key_Shift)
			buffKey[5] = 1;

	}
	if (e->type() == QEvent::KeyRelease) {
		QKeyEvent *ke = static_cast<QKeyEvent *>(e);
		if (ke->key() == Qt::Key_Left)
			buffKey[2] = 0;
		
		if (ke->key() == Qt::Key_Right)
			buffKey[3] = 0;
		
		if (ke->key() == Qt::Key_Up)
			buffKey[0] = 0;

		if (ke->key() == Qt::Key_Down)
			buffKey[1] = 0;

		if (ke->key() == Qt::Key_Space)
			buffKey[4] = 0;

		if (ke->key() == Qt::Key_Q)
			buffKey[2] = 0;

		if (ke->key() == Qt::Key_D)
			buffKey[3] = 0;

		if (ke->key() == Qt::Key_Z)
			buffKey[0] = 0;

		if (ke->key() == Qt::Key_S)
			buffKey[1] = 0;

		if (ke->key() == Qt::Key_Shift)
			buffKey[5] = 0;
	}

	return QWidget::event(e);
}
