#include "application.h"

#include <iostream>

#include "../helpers/config.h"
#include "systems/achievement.h"
#include "utils/BigNumber.h"

static void glfwErrorCallback(int error, const char* description) {
	fmt::println(stderr, "[GLFW Error {}]: {}", error, description);
}

#if !defined(__APPLE__)
static void glDebugCallback(GLenum source,
		GLenum type, GLuint id, GLenum severity,
		GLsizei , const GLchar* message, const void*) {
	(void)source;
	(void)type;
	(void)id;
	(void)severity;
	if (type == GL_DEBUG_TYPE_OTHER) return;
	fmt::println(stderr, "[GL DEBUG MESSAGE]: {}", message);
}
#endif

std::optional<Application> Application::init(const CommandLineOptions& options) noexcept {
	if (options.help()) return std::nullopt;

	Application self;
	fmt::println("\"{}\" version {}.{}", PROJECT_NAME, VERSION_MAJOR, VERSION_MINOR);

	if (options.x11()) glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);
	
	glfwSetErrorCallback(glfwErrorCallback);
	if (!glfwInit()) {
		fmt::println(stderr, "Failed to initialize GLFW");
		return std::nullopt;
	}

	std::optional<Window*> win = Window::init(1280, 720, PROJECT_NAME);
	if (!win) {
		glfwTerminate();
		return std::nullopt;
	}
	self.m_window = std::move(*win);

	if (gl3wInit()) {
		fmt::println(stderr, "Failed to initialize OpenGL context");
		self.m_window->deinit();
		glfwTerminate();
		return std::nullopt;
	}
	fmt::println("Loaded OpenGL {} & GLSL {}", (char*)glGetString(GL_VERSION), (char*)glGetString(GL_SHADING_LANGUAGE_VERSION));

#if !defined(__APPLE__)
	glDebugMessageCallback(glDebugCallback, nullptr);
#endif

	self.m_audio_engine = new AudioSystem;
	*self.m_audio_engine = AudioSystem::init();

	self.m_discord = Discord::init();
	self.m_discord.update();
	self.m_registry = Registry::init();
	self.m_draw = Draw::init(self.m_window, self.m_registry);

	self.m_entity_factory = EntityFactory::init(self.m_window, self.m_registry);

	self.m_world = WorldSystem::init(self.m_window, self.m_registry, self.m_audio_engine);
	
	self.m_ui = new UISystem;
	*self.m_ui = UISystem::init(self.m_window, self.m_registry);
	self.m_ui->initWindowSwitchButtons();
	self.m_ui->initBaseButtons();
	self.m_ui->initButtons();

	self.m_render = RenderSystem::init(self.m_window, self.m_registry, self.m_ui, self.m_draw);
	self.m_camera = CameraSystem::init(self.m_registry);
	self.m_field = FieldSystem::init(self.m_window, self.m_registry, self.m_ui);
	self.m_tools = ToolSystem::init(self.m_window, self.m_registry, self.m_ui);
	self.m_market = MarketSystem::init(self.m_window, self.m_registry, self.m_ui);
	self.m_helper = HelperSystem::init(self.m_window, self.m_registry, self.m_ui);
	self.m_unlock = UnlockSystem::init(self.m_window, self.m_registry, self.m_ui);
	self.m_achievement = AchievementSystem::init(self.m_window, self.m_registry, self.m_ui);
	self.m_gamble = GambleSystem::init(self.m_window, self.m_registry, self.m_ui);
	self.m_skilltree = SkilltreeSystem::init(self.m_window, self.m_registry);

	return self;
}

void Application::deinit() noexcept {
	m_world.deinit();
	m_render.deinit();
	m_audio_engine->deinit();
	m_field.deinit();
	m_market.deinit();
	m_tools.deinit();
	m_helper.deinit();
	m_unlock.deinit();
	m_achievement.deinit();
	m_gamble.deinit();
	m_skilltree.deinit();

	m_draw->deinit();

	delete m_registry;
	m_window->deinit();
	delete m_window;
	glfwTerminate();
}

void Application::run() noexcept {

	ma_sound* music = m_audio_engine->backgroundMusic();
	ma_sound_set_looping(music, MA_TRUE);
	ma_sound_start(music);

	m_window->setResizeCallback([&](GLFWwindow* window, int width, int height) -> void {
			onResizeCallback(window, width, height);
			});
	m_window->setKeyCallback([&](GLFWwindow* window, int key, int scancode, int action, int mods) -> void {
			onKeyCallback(window, key, scancode, action, mods);
			});
    m_window->setMouseCallback([&](GLFWwindow* window, int key, int action, int mods) -> void {
        onMouseCallback(window, key, action, mods);
    });
	m_window->setMouseClickCallback([&](GLFWwindow* window, int key, int action, int mods) -> void {
		onMouseClickCallback(window, key, action, mods);
	});
	
	float delta_time = 0;
	float last_time = static_cast<float>(glfwGetTime());

	glClearColor(0, 0, 0, 1);
	while (!m_window->shouldClose()) {
		glfwPollEvents();

		if (m_world.shouldReset()) reset();

		float time = static_cast<float>(glfwGetTime());
		delta_time = time - last_time;
		last_time = time;


		m_unlock.step(delta_time);

		m_world.step(delta_time);

		m_field.step(delta_time);

		m_market.step(delta_time);

		m_tools.step(delta_time);

		m_helper.step(delta_time);

		m_achievement.step(delta_time);

		m_gamble.step(delta_time);

		m_skilltree.step(delta_time);

		m_ui->step(delta_time);

		m_camera.step(delta_time);

		m_draw->start();
		m_render.step(delta_time);
		m_draw->finish();

		m_window->swapBuffers();
	}

	std::cout<<"Saving..."<<std::endl;
	m_entity_factory.save_values();
}

void Application::reset() noexcept {
	// reset all game state
	m_registry->ecs.clear();

	StatsManager::instance().init();

	// reset bools
	m_registry->debug_mode = false;

	entt::entity money = m_registry->create_money();
	m_registry->ecs.emplace<Amount>(money);

	m_registry->chips = m_registry->ecs.create();
	m_registry->ecs.emplace<Position>(m_registry->chips);

	// Reset world before factory
	m_world.reset();

	/**
	 * Load Base Values
	 */
	m_entity_factory.loadBaseValues();


	/**
	 * Make tests
	 */
	//tests();

	m_field.reset();
	m_market.reset();
	m_tools.reset();
	m_helper.reset();
	m_gamble.reset();

	/**
	 * Reset UI
	 */
	m_ui->reset();
	m_ui->initButtons();

	m_achievement.init_achievement_hover();

	// Reset Field
	m_registry->field_mode = FIELD_FARMING;

	// reset other systems
	m_render.reset();
	m_camera.reset();
	m_unlock.reset();
	m_achievement.reset();
}

void Application::changeWindow() noexcept {

}


void Application::onKeyCallback(GLFWwindow* /*window*/, int key, int /*scancode*/, int action, int /*mods*/) noexcept {
	// preserve “world-first” behavior: Input state is updated before UI/tut see it
	m_registry->on_key_callback(key, action);
}

void Application::onMouseCallback(GLFWwindow* /*window*/, int button, int action, int /*mods*/) noexcept {
	// Preserve old behavior: mouse_left is only updated while IN_GAME
	m_registry->on_mouse_callback(button, action);
}

void Application::onResizeCallback(GLFWwindow* window, int width, int height) noexcept {
	m_draw->onResizeCallback(window, width, height);
}

void Application::onMouseClickCallback(GLFWwindow *window, int button, int action, int mods) noexcept {
	(void) window;
	(void) mods;

	if (m_registry->can_click && action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_1) {
		// Left Click
		bool click_found = false;
		if (m_registry->window() == FIELDS) {
			click_found = m_field.left_click();
		} else if (m_registry->window() == MARKETPLACE) {
			click_found = m_market.left_click();
		} else if (m_registry->window() == TOOLSMITH) {
			click_found = m_tools.left_click();
		} else if (m_registry->window() == HELPERS) {
			click_found = m_helper.left_click();
		} else if (m_registry->window() == GAMBLING) {
			click_found = m_gamble.left_click();
		}

		if (!click_found) {
			m_ui->left_click();
		}
	}


}
