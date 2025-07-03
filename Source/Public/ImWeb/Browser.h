#pragma once

#include <litehtml.h>
#include <map>

// ImGui Forward Declarations
struct ImFont;

namespace aby::web {

	class Logger {
	public:
		virtual void err(const std::string& msg) = 0;
		virtual void log(const std::string& msg) = 0;
		virtual void warn(const std::string& msg) = 0;
 	private:
	};

	class TextureHandler {
	public:
		virtual void load(const char* src, const char* baseurl) = 0;
	private:
	};

    namespace lh   = litehtml;

    class Document : public lh::document_container {
    public:
		Document(std::unique_ptr<Logger> logger = nullptr, void* native_window);
		~Document();

		lh::uint_ptr	    create_font(const lh::font_description& descr, const lh::document* doc, lh::font_metrics* fm) override;
		void				delete_font(lh::uint_ptr hFont) override;
		int					text_width(const char* text, lh::uint_ptr hFont) override;
		void				draw_text(lh::uint_ptr hdc, const char* text, lh::uint_ptr hFont, lh::web_color color, const lh::position& pos) override;
		int					pt_to_px(int pt) const override;
		int					get_default_font_size() const override;
		const char*			get_default_font_name() const override;
		void				draw_list_marker(lh::uint_ptr hdc, const lh::list_marker& marker) override;
		void				load_image(const char* src, const char* baseurl, bool redraw_on_ready) override;
		void				get_image_size(const char* src, const char* baseurl, lh::size& sz) override;
		void				draw_image(lh::uint_ptr hdc, const lh::background_layer& layer, const std::string& url, const std::string& base_url) override;
		void				draw_solid_fill(lh::uint_ptr hdc, const lh::background_layer& layer, const lh::web_color& color) override;
		void				draw_linear_gradient(lh::uint_ptr hdc, const lh::background_layer& layer, const lh::background_layer::linear_gradient& gradient) override;
		void				draw_radial_gradient(lh::uint_ptr hdc, const lh::background_layer& layer, const lh::background_layer::radial_gradient& gradient) override;
		void				draw_conic_gradient(lh::uint_ptr hdc, const lh::background_layer& layer, const lh::background_layer::conic_gradient& gradient) override;
		void				draw_borders(lh::uint_ptr hdc, const lh::borders& borders, const lh::position& draw_pos, bool root) override;

		void				set_caption(const char* caption) override;
		void				set_base_url(const char* base_url) override;
		void				link(const std::shared_ptr<lh::document>& doc, const lh::element::ptr& el) override;
		void				on_anchor_click(const char* url, const lh::element::ptr& el) override;
		bool				on_element_click(const lh::element::ptr& el);
		void				on_mouse_event(const lh::element::ptr& el, lh::mouse_event event) override;
		void				set_cursor(const char* cursor) override;
		void				transform_text(lh::string& text, lh::text_transform tt) override;
		void				import_css(lh::string& text, const lh::string& url, lh::string& baseurl) override;
		void				set_clip(const lh::position& pos, const lh::border_radiuses& bdr_radius) override;
		void				del_clip() override;
		void				get_viewport(lh::position& viewport) const override;
		lh::element::ptr	create_element(const char* tag_name, const lh::string_map& attributes, const std::shared_ptr<lh::document>& doc) override;

		void				get_media_features(lh::media_features& media) const override;
		void				get_language(lh::string& language, lh::string& culture) const override;
	private:
		std::unique_ptr<Logger> m_Logger;
		void* m_Window;
	};


}