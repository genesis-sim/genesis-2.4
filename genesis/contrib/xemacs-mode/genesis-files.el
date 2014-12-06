;;; genesis-files.el --- Bind specific file types to the Genesis mode.

;; Copyright (C) 2005-2006 Hugo Cornelis

;; Authors:    2005-2006 Hugo Cornelis
;; Maintainer: Hugo Cornelis

;; You should have received a copy of the GNU General Public License
;; along with GNU Emacs; see the file COPYING.  If not, write to the
;; Free Software Foundation, Inc., 59 Temple Place - Suite 330,
;; Boston, MA 02111-1307, USA.

;; recognise .g files as genesis files
(setq auto-mode-alist
      (append '(
                ("\\.g$" . genesis-mode))
              auto-mode-alist))

;; recognise .p files as c++ files
(setq auto-mode-alist
      (append '(
                ("\\.p$" . c++-mode))
              auto-mode-alist))

;; configure function menu for genesis mode
(defvar fume-function-name-regexp-genesis
  "^\\(function\\|extern\\)[ \t]+\\([_a-zA-Z][_a-zA-Z0-9]*\\)"
  "Expression to get function/extern names in Genesis scripts.")

(setq fume-function-name-regexp-alist
      (append '((genesis-mode . fume-function-name-regexp-genesis))
              fume-function-name-regexp-alist))

;; Find next genesis function in the buffer, derived from pascal version.
;; Hugo Cornelis <hugo.cornelis@gmail.com>
;;
(defun fume-find-next-genesis-function-name (buffer)
  "Search for the next Genesis function in BUFFER."
  (set-buffer buffer)
  (if (re-search-forward fume-function-name-regexp-genesis nil t)
      (let ((beg (match-beginning 2))
            (end (match-end 2)))
        (cons (buffer-substring beg end) beg))))

(setq fume-find-function-name-method-alist
      (append '((genesis-mode . fume-find-next-genesis-function-name))
              fume-find-function-name-method-alist))


