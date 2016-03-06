;; define several category of keywords
(setq fl-keywords '("exit" "defclass" "defenum" "observe" "raise" "handle" "package" "while" "default" "trace" "eval" "debug" "show" "cd" "render" "quit" "include" "null" "true" "false") )
(setq fl-types '("object" "float" "integer" "void" "boolean" "string" "signal" "list" "lazy" "enum" "evalsignal" "function" "class" ))

(setq fl-constants '())
(setq fl-functions '("I" "head" "tail" "convert" "addmethod" "constructor" "addattr" "size" "foreach" "rnd" "print" "dump" "eq" "invoke" "can_convert" "methods" "attributes" "base" "new" "member" "add" "join" "in_range" "gt" "lt" "dec" "div" "divf" "mod" "append" "prepend" "pop" "not"))

;; generate regex string for each category of keywords
(setq fl-keywords-regexp (regexp-opt fl-keywords 'words))
(setq fl-type-regexp (regexp-opt fl-types 'words))
(setq fl-constant-regexp (regexp-opt fl-constants 'words))
(setq fl-functions-regexp (regexp-opt fl-functions 'words))

;; create the list for font-lock.
;; each category of keyword is given a particular face
(setq fl-font-lock-keywords
      `(
	("#.*$\\|//.*$" . font-lock-comment-face)
        (,fl-type-regexp . font-lock-type-face)
        (,fl-constant-regexp . font-lock-constant-face)
        (,fl-functions-regexp . font-lock-function-name-face)
        (,fl-keywords-regexp . font-lock-keyword-face)
        ;; note: order above matters, because once colored, that part won't change.
        ;; in general, longer words first
        ))

;;;###autoload
(define-derived-mode fl-mode fundamental-mode
  "fl mode"
  "Major mode for editing fl (github.com/team-fl/)…"

  ;; code for syntax highlighting
  (setq font-lock-defaults '((fl-font-lock-keywords)))
)

;; clear memory. no longer needed
(setq fl-keywords nil)
(setq fl-types nil)
(setq fl-constants nil)
(setq fl-functions nil)

;; clear memory. no longer needed
(setq fl-keywords-regexp nil)
(setq fl-types-regexp nil)
(setq fl-constants-regexp nil)
(setq fl-functions-regexp nil)

;; add the mode to the `features' list
(provide 'emacs-fl-mode)

;; Local Variables:
;; coding: utf-8
;; End:

;;; fl-mode.el ends here
