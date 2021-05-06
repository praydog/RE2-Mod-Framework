#pragma once

#include <spdlog/spdlog.h>

class Mods;
class REGlobals;
class RETypes;

#include "D3D11Hook.hpp"
#include "D3D12Hook.hpp"
#include "DInputHook.hpp"
#include "WindowsMessageHook.hpp"

// Global facilitator
class REFramework {
public:
    REFramework();
    virtual ~REFramework();

    bool is_valid() const { return m_valid; }

    bool is_dx11() const { return m_is_d3d11; }

    bool is_dx12() const { return m_is_d3d12; }

    const auto& get_mods() const { return m_mods; }

    const auto& get_types() const { return m_types; }

    const auto& get_mouse_delta() const { return m_mouse_delta; }
    const auto& get_keyboard_state() const { return m_last_keys; }

    const auto& get_globals() const { return m_globals; }

    Address get_module() const { return m_game_module; }

    bool is_ready() const { return m_game_data_initialized; }

    void on_frame_d3d11();
    void on_frame_d3d12();
    void on_reset();
    bool on_message(HWND wnd, UINT message, WPARAM w_param, LPARAM l_param);
    void on_direct_input_keys(const std::array<uint8_t, 256>& keys);

    void save_config();

private:
    void consume_input();

    void draw_ui();
    void draw_about();

    bool hook_d3d11();
    bool hook_d3d12();

    bool initialize();

    bool m_first_frame{true};
    bool m_is_d3d12{false};
    bool m_is_d3d11{false};
    bool m_valid{false};
    bool m_initialized{false};
    bool m_draw_ui{true};
    std::atomic<bool> m_game_data_initialized{false};

    std::mutex m_input_mutex{};

    HWND m_wnd{0};
    HMODULE m_game_module{0};
    uint8_t m_menu_key{DIK_INSERT};

    float m_accumulated_mouse_delta[2]{};
    float m_mouse_delta[2]{};
    std::array<uint8_t, 256> m_last_keys{0};
    std::unique_ptr<D3D11Hook> m_d3d11_hook{};
    std::unique_ptr<D3D12Hook> m_d3d12_hook{};
    std::unique_ptr<WindowsMessageHook> m_windows_message_hook;
    std::unique_ptr<DInputHook> m_dinput_hook;
    std::shared_ptr<spdlog::logger> m_logger;

    std::string m_error{""};

    // Game-specific stuff
    std::unique_ptr<Mods> m_mods;
    std::unique_ptr<REGlobals> m_globals;
    std::unique_ptr<RETypes> m_types;

private: // D3D11 Init
    void create_render_target_d3d11();
    void cleanup_render_target_d3d11();

private: // D3D12 Init
    bool create_rtv_descriptor_heap_d3d12();
    bool create_srv_descriptor_heap_d3d12();
    bool create_command_allocator_d3d12();
    bool create_command_list_d3d12();
    void cleanup_render_target_d3d12();
    void create_render_target_d3d12();

private: // D3D11 members
    ID3D11RenderTargetView* m_main_render_target_view_d3d11{nullptr};

private: // D3D12 members
    struct FrameContextD3D12 {
        ID3D12CommandAllocator* command_allocator;
        UINT64 fence_value;
    };

    static constexpr int s_NUM_FRAMES_IN_FLIGHT_D3D12{3};
    static constexpr int s_NUM_BACK_BUFFERS_D3D12{3};

    FrameContextD3D12 m_frame_context_d3d12[s_NUM_FRAMES_IN_FLIGHT_D3D12]{};
    ID3D12DescriptorHeap* m_pd3d_rtv_desc_heap_d3d12{nullptr};
    ID3D12DescriptorHeap* m_pd3d_srv_desc_heap_d3d12{nullptr};
    ID3D12CommandQueue* m_pd3d_command_queue_d3d12{nullptr};
    ID3D12GraphicsCommandList* m_pd3d_command_list_d3d12{nullptr};
    ID3D12Resource* m_main_render_target_resource_d3d12[s_NUM_BACK_BUFFERS_D3D12]{nullptr};
    D3D12_CPU_DESCRIPTOR_HANDLE m_main_render_target_descriptor_d3d12[s_NUM_BACK_BUFFERS_D3D12]{};
};

extern std::unique_ptr<REFramework> g_framework;