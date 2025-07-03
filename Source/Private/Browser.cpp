#include "ImWeb/Browser.h"
#include <imgui.h>
#include <format>



#define ERR(fmt, ...) m_Logger->err(std::format(fmt __VA_OPT__(,) __VA_ARGS__))
#define LOG(fmt, ...) m_Logger->log(std::format(fmt __VA_OPT__(,) __VA_ARGS__))
#define WARN(fmt, ...) m_Logger->warn(std::format(fmt __VA_OPT__(,) __VA_ARGS__))

#ifdef _WIN32
    #include <Windows.h>
    #undef min
    #undef max
#else
    #error "Unsupported OS"
#endif

#define IM_FONT(font) reinterpret_cast<ImFont*>(font)
#define IM_COLOR(color) IM_COL32(color.red, color.green, color.blue, color.alpha)
namespace aby::web {
	
		Document::Document(std::unique_ptr<Logger> logger, void* native_window) : 
			m_Logger(std::move(logger)),
            m_Window(native_window)
        {
        }

		Document::~Document() {
        }

        lh::uint_ptr Document::create_font(const lh::font_description& descr, const lh::document* doc, lh::font_metrics* fm) {
            LOG("Document::create_font(...)");

            ImFontAtlas* fonts = ImGui::GetIO().Fonts;
            std::string font_name = descr.hash();

            ImFontConfig config;
            config.SizePixels = descr.size;
            config.OversampleH = 2;
            config.OversampleV = 2;
            config.PixelSnapH = false;
            ImFont* font = fonts->AddFontFromFileTTF(font_name.c_str(), descr.size, &config);
            if (!font) {
                ERR("ImGui::GetIO().Fonts->AddFontFromFileTTF(...) failed to load font");
                return 0;
            }
            if (!font->LastBaked) {
                ERR("Font was not baked yet");
                return;
            }

            float ascent = font->LastBaked->Ascent;
            float descent = font->LastBaked->Descent;
            float height = descr.size - (ascent - descent);

            fm->font_size = descr.size;
            fm->height    = height;
            fm->ascent    = ascent;
            fm->descent   = descent;
            ImGui::PushFont(font);
            fm->x_height  = static_cast<int>(ImGui::CalcTextSize("x").y);
            fm->ch_width  = static_cast<int>(ImGui::CalcTextSize("0").x);
            ImGui::PopFont();
            fm->draw_spaces = true;
            fm->sub_shift = 0; // TODO: Figure out subshift of font
            fm->super_shift = 0; // TODO: Figure out supershift of font


            return reinterpret_cast<lh::uint_ptr>(font);
        }

		void Document::delete_font(lh::uint_ptr hFont) {
            LOG("Document::delete_font(...)");

            ImGui::GetIO().Fonts->RemoveFont(IM_FONT(hFont));
        }
		
        int	Document::text_width(const char* text, lh::uint_ptr hFont) {
            LOG("Document::text_width(...)");

            ImFont* font = IM_FONT(hFont);
            int width = 0;
            if (ImGui::GetFont() != font) {
                ImGui::PushFont(font);
                width = static_cast<int>(ImGui::CalcTextSize(text).x);
                ImGui::PopFont();
            } else {
                width = static_cast<int>(ImGui::CalcTextSize(text).x);
            }
            return width;
        }

		void Document::draw_text(lh::uint_ptr hdc, const char* text, lh::uint_ptr hFont, lh::web_color color, const lh::position& pos) {
            LOG("Document::draw_text(...)");

            ImDrawList* draw_list = ImGui::GetWindowDrawList();
            ImFont* font = IM_FONT(hFont);
            float font_size = font->LastBaked->Size;
            ImGui::PushFont(font);
            draw_list->AddText(font, font_size, ImVec2(pos.x, pos.y), IM_COLOR(color), text);
            ImGui::PopFont();
        }

        int Document::pt_to_px(int pt) const {
#ifdef _WIN32
            auto hwnd = static_cast<HWND>(m_Window);
            UINT dpi = GetDpiForWindow(hwnd);
            float px = pt * (dpi / 72.0f);
#else
            float px = pt * 1.333f; // Fallback for non-Windows platforms (assume 96 DPI)
#endif
            return static_cast<int>(px);
        }

		int	Document::get_default_font_size() const { 
            auto font = ImGui::GetIO().Fonts->Fonts[0];
            return static_cast<int>(font->LastBaked->Size);
        }
		
        const char* Document::get_default_font_name() const { 
            auto font = ImGui::GetIO().Fonts->Fonts[0];
            return font->GetDebugName();
        }
		
        void Document::draw_list_marker(lh::uint_ptr hdc, const lh::list_marker& marker) {
            LOG("Document::draw_list_marker(...)");

            ImGui::PushFont(IM_FONT(marker.font));

            int top_margin = marker.pos.height / 3;
            if (top_margin < 4)
                top_margin = 0;

            int draw_x = marker.pos.x + marker.pos.width / 2;
            int draw_y = marker.pos.y + top_margin + (marker.pos.height - top_margin * 2) / 2;
            int draw_size = std::min(marker.pos.width, marker.pos.height - top_margin * 2);

            ImDrawList* draw_list = ImGui::GetWindowDrawList();
            ImU32 color = IM_COLOR(marker.color);

            switch (marker.marker_type) {
                case lh::list_style_type_circle: // Fallthrough
                case lh::list_style_type_disc: {
                    ImVec2 center(draw_x, draw_y);
                    float radius = 0.5f * draw_size;
                    draw_list->AddCircleFilled(center, radius, color); 
                    break;
                }
                case lh::list_style_type_square: {
                    float half = 0.5f * draw_size;
                    ImVec2 min(draw_x - half, draw_y - half);
                    ImVec2 max(draw_x + half, draw_y + half);
                    draw_list->AddRectFilled(min, max, color);
                    break;
                }
                default:
                    throw std::out_of_range("litehtml::list_style_type");
            }

            ImGui::PopFont();
        }

		void Document::load_image(const char* src, const char* baseurl, bool redraw_on_ready) {
            LOG("Document::draw_list_marker(src: {}, baseurl: {}, ...)", src, baseurl);
        }

        void Document::get_image_size(const char* src, const char* baseurl, lh::size& sz) {
            LOG("Document::get_image_size(src: {}, baseurl: {})", src, baseurl);
        }

        void Document::draw_image(lh::uint_ptr hdc, const lh::background_layer& layer, const std::string& url, const std::string& base_url) {
            LOG("Document::draw_image(url: {}, base_url: {})", url, base_url);
        }

        void Document::draw_solid_fill(lh::uint_ptr hdc, const lh::background_layer& layer, const lh::web_color& color) {
            LOG("Document::draw_solid_fill(...)");
        }

        void Document::draw_linear_gradient(lh::uint_ptr hdc, const lh::background_layer& layer, const lh::background_layer::linear_gradient& gradient) {
            LOG("Document::draw_linear_gradient(...)");
        }

        void Document::draw_radial_gradient(lh::uint_ptr hdc, const lh::background_layer& layer, const lh::background_layer::radial_gradient& gradient) {
            LOG("Document::draw_radial_gradient(...)");
        }

        void Document::draw_conic_gradient(lh::uint_ptr hdc, const lh::background_layer& layer, const lh::background_layer::conic_gradient& gradient) {
            LOG("Document::draw_conic_gradient(...)");
        }

        void Document::draw_borders(lh::uint_ptr hdc, const lh::borders& borders, const lh::position& draw_pos, bool root) {
            LOG("Document::draw_borders(...)");
        }

        void Document::set_caption(const char* caption) {
            LOG("Document::set_caption(caption: {})", caption);
        }

        void Document::set_base_url(const char* base_url) {
            LOG("Document::set_base_url(base_url: {})", base_url);
        }

        void Document::link(const std::shared_ptr<lh::document>& doc, const lh::element::ptr& el) {
            LOG("Document::link(...)");
        }

        void Document::on_anchor_click(const char* url, const lh::element::ptr& el) {
            LOG("Document::on_anchor_click(url: {})", url);
        }

        bool Document::on_element_click(const lh::element::ptr& el) {
            LOG("Document::on_element_click(...)");
            return false;
        }

        void Document::on_mouse_event(const lh::element::ptr& el, lh::mouse_event event) {
            LOG("Document::on_mouse_event(...)");
        }

        void Document::set_cursor(const char* cursor) {
            LOG("Document::set_cursor(cursor: {})", cursor);
        }

        void Document::transform_text(lh::string& text, lh::text_transform tt) {
            LOG("Document::transform_text(...)");
        }

        void Document::import_css(lh::string& text, const lh::string& url, lh::string& baseurl) {
            LOG("Document::import_css(url: {}, baseurl: {})", url, baseurl);
        }

        void Document::set_clip(const lh::position& pos, const lh::border_radiuses& bdr_radius) {
            LOG("Document::set_clip(...)");
        }

        void Document::del_clip() {
            LOG("Document::del_clip(...)");
        }

        void Document::get_viewport(lh::position& viewport) const {
            LOG("Document::get_viewport(...)");
        }

        lh::element::ptr Document::create_element(const char* tag_name, const lh::string_map& attributes, const std::shared_ptr<lh::document>& doc) {
            LOG("Document::create_element(tag_name: {})", tag_name);
            return nullptr;
        }

        void Document::get_media_features(lh::media_features& media) const {
            LOG("Document::get_media_features(...)");
        }

        void Document::get_language(lh::string& language, lh::string& culture) const {
            LOG("Document::get_language(...)");
        }
}