;;; edje site-lisp configuration

(add-to-list 'load-path "@SITELISP@")
(add-to-list 'auto-mode-alist '("\\.edc$" . edje-mode))
(autoload 'edje-mode "edje-mode" "Edje source file syntax highlight and programming helpers")
