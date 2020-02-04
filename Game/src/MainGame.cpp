#include <Oyl3D.h>

using namespace oyl;
using namespace component;

class MainLayer : public Layer
{
public:
    OYL_CTOR(MainLayer, Layer)

    glm::vec3 m_cameraMove      = glm::vec3(0.0f);
    glm::vec3 m_cameraRotate    = glm::vec3(0.0f);
    float     m_cameraMoveSpeed = 15;
    bool      m_doMoveCamera    = false;

    PostProcessingPass m_warm;
    PostProcessingPass m_cold;
    PostProcessingPass m_cust;

    PostProcessingPass* m_currPass;

    void onEnter() override
    {
        listenForEventType(EventType::KeyPressed);
        listenForEventType(EventType::KeyReleased);
        listenForEventType(EventType::MouseMoved);

        #if defined(OYL_DISTRIBUTION)
            m_doMoveCamera = true;
            CursorStateRequestEvent request;
            request.state = CursorState::Disabled;
            postEvent(request);
        #endif

        auto shader = Shader::cache(Shader::create({
            { Shader::Type::Vertex,   "res/assets/shaders/passthrough.vert" },
            { Shader::Type::Fragment, "res/assets/shaders/postLUT.frag" }
        }), "LUT");

        m_warm.shader = shader;
        m_warm.setUniformTexture3D("u_lut", Texture3D::create("res/assets/textures/WARM_LUT.CUBE"));
        
        m_cold.shader = shader;
        m_cold.setUniformTexture3D("u_lut", Texture3D::create("res/assets/textures/COLD_LUT.CUBE"));

        m_cust.shader = shader;
        m_cust.setUniformTexture3D("u_lut", Texture3D::create("res/assets/textures/CUST_LUT.CUBE"));
    }

    void onUpdate() override
    {
        auto view = registry->view<Transform, Camera>();

        view.each([&](Transform& transform, Camera& camera)
        {
            if (!m_doMoveCamera)
            {
                m_cameraMove   = glm::vec3(0.0f);
                m_cameraRotate = glm::vec3(0.0f);
                return;
            }

            if (transform.hasParent())
            {
                transform.getParent()->translate(transform.getForwardGlobal() * m_cameraMove.z * Time::deltaTime(), false);
                transform.getParent()->translate(transform.getUpGlobal()      * m_cameraMove.y * Time::deltaTime(), false);
                transform.getParent()->translate(transform.getRightGlobal()   * m_cameraMove.x * Time::deltaTime(), false);
                transform.getParent()->rotate(glm::vec3(0, m_cameraRotate.y, 0));
            }

            transform.rotate(glm::vec3(m_cameraRotate.x, 0, 0));

            if (transform.getRotationEulerX() > 89.0f)
                transform.setRotationEulerX(89.0f);
            if (transform.getRotationEulerX() < -89.0f)
                transform.setRotationEulerX(-89.0f);

            m_cameraRotate = glm::vec3(0.0f);
        });
    }

    bool onEvent(const Event& event) override
    {
        switch (event.type)
        {
            case EventType::KeyPressed:
            {
                auto e = event_cast<KeyPressedEvent>(event);
                if (!e.repeatCount)
                {
                    if (m_doMoveCamera)
                    {
                        if (e.keycode == Key::W)
                            m_cameraMove.z += m_cameraMoveSpeed;
                        if (e.keycode == Key::S)
                            m_cameraMove.z -= m_cameraMoveSpeed;
                        if (e.keycode == Key::D)
                            m_cameraMove.x += m_cameraMoveSpeed;
                        if (e.keycode == Key::A)
                            m_cameraMove.x -= m_cameraMoveSpeed;
                        if (e.keycode == Key::Space)
                            m_cameraMove.y += m_cameraMoveSpeed;
                        if (e.keycode == Key::LeftControl)
                            m_cameraMove.y -= m_cameraMoveSpeed;
                    }

                    if (e.keycode == Key::Escape) hideCursor();

                    if (e.keycode == Key::Alpha8)
                        setPostPass(&m_warm);
                    if (e.keycode == Key::Alpha9)
                        setPostPass(&m_cold);
                    if (e.keycode == Key::Alpha0)
                        setPostPass(&m_cust);
                }
                break;
            }
            case EventType::KeyReleased:
            {
                if (m_doMoveCamera)
                {
                    auto e = event_cast<KeyReleasedEvent>(event);
                    if (e.keycode == Key::W)
                        m_cameraMove.z -= m_cameraMoveSpeed;
                    if (e.keycode == Key::S)
                        m_cameraMove.z += m_cameraMoveSpeed;
                    if (e.keycode == Key::D)
                        m_cameraMove.x -= m_cameraMoveSpeed;
                    if (e.keycode == Key::A)
                        m_cameraMove.x += m_cameraMoveSpeed;
                    if (e.keycode == Key::Space)
                        m_cameraMove.y -= m_cameraMoveSpeed;
                    if (e.keycode == Key::LeftControl)
                        m_cameraMove.y += m_cameraMoveSpeed;
                }
                break;
            }
            case EventType::MouseMoved:
            {
                auto e = event_cast<MouseMovedEvent>(event);

                m_cameraRotate.y = -e.dx;
                m_cameraRotate.x = -e.dy;

                break;
            }
        }
        return false;
    }

    void hideCursor()
    {
        CursorStateRequestEvent cursorRequest;
        if (!m_doMoveCamera)
        {
            listenForEventType(EventType::MouseMoved);
            listenForEventType(EventType::KeyReleased);
            cursorRequest.state = CursorState::Disabled;
        } else
        {
            ignoreEventType(EventType::MouseMoved);
            ignoreEventType(EventType::KeyReleased);
            cursorRequest.state = CursorState::Normal;
        }

        postEvent(cursorRequest);
        m_doMoveCamera ^= true;
    }

    void setPostPass(PostProcessingPass* pass = nullptr)
    {
        if (pass != m_currPass)
            m_currPass = pass;
        else
            m_currPass = pass = nullptr;
        
        registry->view<Camera>().each([&](Camera& camera)
        {
            if (!pass)
            {
                camera.postProcessingPasses.clear();
            }
            else if (pass == &m_warm)
            {
                camera.postProcessingPasses.resize(1);
                camera.postProcessingPasses[0] = m_warm;
            }
            else if (pass == &m_cold)
            {
                camera.postProcessingPasses.resize(1);
                camera.postProcessingPasses[0] = m_cold;
            }
            else if (pass == &m_cust)
            {
                camera.postProcessingPasses.resize(1);
                camera.postProcessingPasses[0] = m_cust;
            }
        });
    }
};

class MainScene : public Scene
{
public:
    OYL_CTOR(MainScene, Scene)

    virtual void onEnter() override
    {
        pushLayer(MainLayer::create());
    }
};

class Game : public oyl::Application
{
public:
    Game()
    {
        pushScene(MainScene::create());
    }

    virtual void onExit() { }
};

oyl::Application* oyl::createApplication()
{
    return new Game();
}
