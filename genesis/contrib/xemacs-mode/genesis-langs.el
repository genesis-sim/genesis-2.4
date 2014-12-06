;;; genesis-langs.el --- specific language support for Genesis Mode

;; Copyright (C) 2005-2006 Hugo Cornelis

;; Authors:    2005-2006 Hugo Cornelis
;; Maintainer: Hugo Cornelis

;; You should have received a copy of the GNU General Public License
;; along with GNU Emacs; see the file COPYING.  If not, write to the
;; Free Software Foundation, Inc., 59 Temple Place - Suite 330,
;; Boston, MA 02111-1307, USA.

(require 'cc-mode)


;; Support for genesis scripts

(defvar genesis-mode-abbrev-table nil
  "Abbreviation table used in genesis-mode buffers.")
(define-abbrev-table 'genesis-mode-abbrev-table ())

(defvar genesis-mode-map ()
  "Keymap used in genesis-mode buffers.")
;;(if genesis-mode-map
;;    nil
;;  (setq genesis-mode-map (c-make-inherited-keymap))
;; ;; add bindings which are only useful for genesis mode
;;  )
(if genesis-mode-map ()
  (setq genesis-mode-map (make-sparse-keymap))
  (set-keymap-name genesis-mode-map 'genesis-mode-map)
;;  (define-key genesis-mode-map "\e\t" 'ispell-complete-word)
  (define-key genesis-mode-map "\t" 'tab-to-tab-stop)
;;  (define-key genesis-mode-map "\es" 'center-line)
;;  (define-key genesis-mode-map "\eS" 'center-paragraph)
  )




;;;###autoload
(defvar genesis-mode-syntax-table nil
  "Syntax table used in genesis-mode buffers.")
(if genesis-mode-syntax-table
    nil
  (setq genesis-mode-syntax-table (make-syntax-table))
  (c-populate-syntax-table genesis-mode-syntax-table)
  )

(easy-menu-define c-genesis-menu genesis-mode-map "Genesis Mode Commands"
		  (c-mode-menu "Genesis"))



(provide 'genesis-langs)
;;; genesis-langs.el ends here
