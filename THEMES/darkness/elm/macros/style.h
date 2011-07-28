#define STYLE(style_class, fn, fb, fi, fbi, fm, size, normal_style, title, subtitle, hilight, description) \
style { \
   name: style_class; \
   base: "font="fn" font_size="size" text_class="style_class" align=left wrap=word "normal_style; \
   tag: "br" "\n"; \
   tag: "ps" "ps"; \
   tag: "tab" "\t"; \
   tag: "left" "+ align=left"; \
   tag: "/left" "-\n"; \
   tag: "center" "+ align=center"; \
   tag: "/center" "- \n"; \
   tag: "right" "+ align=right"; \
   tag: "/right" "- \n"; \
   tag: "link" "+ color=#8888AA underline=on underline_color=#8888AA"; \
   tag: "em" "+ font=DejaVu-Oblique"; \
   tag: "b" "+ font="fb; \
   tag: "i" "+ font="fi; \
   tag: "bi" "+ font="fbi; \
   tag: "mono" "+ font="fm; \
   tag: "title" "+ "title; \
   tag: "subtitle" "+ "subtitle; \
   tag: "hilight" "+ "hilight; \
   tag: "description" "+ "description; \
} \
style { \
   name: style_class"-mixed"; \
   base: "font="fn" font_size="size" text_class="style_class" align=left wrap=mixed "normal_style; \
   tag: "br" "\n"; \
   tag: "ps" "ps"; \
   tag: "tab" "\t"; \
   tag: "left" "+ align=left"; \
   tag: "/left" "-\n"; \
   tag: "center" "+ align=center"; \
   tag: "/center" "- \n"; \
   tag: "right" "+ align=right"; \
   tag: "/right" "- \n"; \
   tag: "link" "+ color=#8888AA underline=on underline_color=#8888AA"; \
   tag: "em" "+ font=DejaVu-Oblique"; \
   tag: "b" "+ font="fb; \
   tag: "i" "+ font="fi; \
   tag: "bi" "+ font="fbi; \
   tag: "mono" "+ font="fm; \
   tag: "title" "+ "title; \
   tag: "subtitle" "+ "subtitle; \
   tag: "hilight" "+ "hilight; \
   tag: "description" "+ "description; \
} \
style { \
   name: style_class"-char"; \
   base: "font="fn" font_size="size" text_class="style_class" align=left wrap=char "normal_style; \
   tag: "br" "\n"; \
   tag: "ps" "ps"; \
   tag: "tab" "\t"; \
   tag: "left" "+ align=left"; \
   tag: "/left" "-\n"; \
   tag: "center" "+ align=center"; \
   tag: "/center" "- \n"; \
   tag: "right" "+ align=right"; \
   tag: "/right" "- \n"; \
   tag: "link" "+ color=#8888AA underline=on underline_color=#8888AA"; \
   tag: "em" "+ font=DejaVu-Oblique"; \
   tag: "b" "+ font="fb; \
   tag: "i" "+ font="fi; \
   tag: "bi" "+ font="fbi; \
   tag: "mono" "+ font="fm; \
   tag: "title" "+ "title; \
   tag: "subtitle" "+ "subtitle; \
   tag: "hilight" "+ "hilight; \
   tag: "description" "+ "description; \
} \
style { \
   name: style_class"-nowrap"; \
   base: "font="fn" font_size="size" text_class="style_class" align=left wrap=none "normal_style; \
   tag: "br" "\n"; \
   tag: "ps" "ps"; \
   tag: "tab" "\t"; \
   tag: "left" "+ align=left"; \
   tag: "/left" "-\n"; \
   tag: "center" "+ align=center"; \
   tag: "/center" "- \n"; \
   tag: "right" "+ align=right"; \
   tag: "/right" "- \n"; \
   tag: "link" "+ color=#8888AA underline=on underline_color=#8888AA"; \
   tag: "em" "+ font=DejaVu-Oblique"; \
   tag: "b" "+ font="fb; \
   tag: "i" "+ font="fi; \
   tag: "bi" "+ font="fbi; \
   tag: "mono" "+ font="fm; \
   tag: "title" "+ "title; \
   tag: "subtitle" "+ "subtitle; \
   tag: "hilight" "+ "hilight; \
   tag: "description" "+ "description; \
}
