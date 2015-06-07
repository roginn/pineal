(import
  [pyglet.gl :as gl]
  [pyglet.graphics [vertex_list]]
  [pyglet.image]
  [pyglet.image.codecs.png [PNGImageDecoder]]
  [lib.windows [getRenderTexture :as _getRenderTexture]]
  [math [cos sin pi]]
  [time [time]]
  [lib.graphic.transforming [*]]
  [lib.graphic.coloring [*]])

(require hy.contrib.multi)


(defun time2rad [&optional [mult 1]]
  "trigonometric functions don't handle really high numbers"
  (% (* mult (time)) (* 2 pi)))


(defun nestle [&rest fs]
    (if (cdr fs)
      ((car fs) (apply nestle (cdr fs)))
      ((car fs))))


(defn build-wired-list [n]
  (vertex_list n
               (tuple ["v2f/static"
                       (flatten
                         (map (fn [i]
                                  (setv theta
                                        (-> (/ pi n)
                                            (* 2 i)))
                                  [(cos theta) (sin theta)])
                              (range n)))])
               (tuple ["c4f/stream"
                       (* [1] 4 n)])))


(defn build-solid-list [n]
  (vertex_list (* n 3)
               (tuple ["v2f/static"
                       (flatten
                         (map (fn [i]
                                  (setv dtheta
                                        (* 2 (/ pi n)))
                                  (setv theta0
                                        (* i dtheta))
                                  (setv theta1
                                        (+ theta0 dtheta))
                                  [ 0 0
                                    (cos theta0) (sin theta0)
                                    (cos theta1) (sin theta1)])
                              (range n)))])
               (tuple ["c4f/stream"
                       (* [1] 4
                          (* n 3))])))

(def memo-solid {})

(defn psolid [n]
  (defn fill [fill-color]
    (fn [&optional f]
        (fn []
            (unless (in n memo-solid)
              (assoc memo-solid n
                     (build-solid-list n)))

            (setv solid-list
                  (get memo-solid n))
            (setv solid-list.colors
                  (* (color fill-color) (* n 3)))

            (.draw solid-list gl.GL_TRIANGLES)
            (when f (f))))))


(def memo-wired {})

(defn pwired [n]
  (defn stroke [stroke-color]
    (fn [&optional f]
        (fn []
            (unless (in n memo-wired)
              (assoc memo-wired n
                     (build-wired-list n)))

            (setv wired-list
                  (get memo-wired n))
            (setv wired-list.colors
                  (* (color stroke-color) n))

            (.draw wired-list gl.GL_LINE_LOOP)
            (when f (f))))))


(defn blit-img [img]
  (.blit img
         -1 1 0
         2 -2))


(defn new-blittable [inner]
  (fn []
      (blit-img inner)))


(defn frame [&optional f]
  (fn []
      (setv texture (_getRenderTexture))
      (when texture (blit-img texture))
      (when f (f))))


(def memo-img {})

(defn image [name]
  "
  Image from png file
  "
  (fn [&optional f]
      (fn []
          (unless (in name memo-img)
            (assoc memo-img name
                   (new-blittable (apply pyglet.image.load
                                         [(+ "images/" name ".png")]
                                         {"decoder" (PNGImageDecoder)}))))
          ((get memo-img name))
          (when f (f)))))
