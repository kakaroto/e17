.function _fractal_outerloop_fp_za
.dest 4 za Eina_F16p16
.source 4 sx
.param 4 twi Eina_F16p16
.temp 4 tx

# Za = (4 * x) / tw - 2 = 4 * x * twi - 2
mulll tx, sx, 4
mulll tx, tx, twi
# 2 in F16p16 is 2 << 16 == 1 << 17
subssl za, tx, 131072
# za is now F16p16 and correctly computed

.function _fractal_outerloop_fp_zb
.dest 4 zb Eina_F16p16
.source 4 sy
.param 4 thi Eina_F16p16
.temp 4 ty

# Zb = (-4 * y) / th + 2 = (-4 * y) * thi + 2
mulll ty, sy, -4
mulll ty, ty, thi
# 2 in F16p16 is 2 << 16 == 1 << 17
addssl zb, ty, 131072
# zb is now computed and in F16p16

.function _fractal_inerloop
.dest 1 kp
.dest 1 test
.dest 4 Zap Eina_F16p16
.dest 4 Zbp Eina_F16p16
.source 1 k
.source 4 Za Eina_F16p16
.source 4 Zb Eina_F16p16
.param 4 Ca Eina_F16p16
.param 4 Cb Eina_F16p16
.temp 4 Zal
.temp 4 Zah
.temp 4 Zbl
.temp 4 Zbh
.temp 4 Zapl
.temp 4 Zaph
.temp 4 Zapt
.temp 4 Zbpt
.temp 4 Zbpt2
.temp 2 tmpl
.temp 2 tmph
.temp 4 Zabl
.temp 4 Zabh
.temp 4 sl
.temp 4 sh
.temp 4 rl
.temp 4 rh
.temp 1 tl
.temp 1 th

# Za' = (Za * Za) - (Zb * Zb) + Ca
# Za' = (((Za * Za) - (Zb * Zb)) >> 16) + Ca [Eina_F16p16]
mulll Zal, Za, Za
mulhul Zah, Za, Za
mulll Zbl, Zb, Zb
mulhul Zbh, Zb, Zb
subl Zapl, Zal, Zbl
subl Zaph, Zah, Zbh
convhlw tmpl, Zapl
convlw tmph, Zaph
mergewl Zapt, tmph, tmpl
addl Zap, Zapt, Ca

# Zb' = Za * Zb * 2 + Cb
# Zb' = ((Za * Zb) >> 16) * 2 + Cb [Eina_F16p16]
mulll Zabl, Za, Zb
mulhsl Zabh, Za, Zb
convhlw tmpl, Zabl
convssslw tmph, Zabh
mergewl Zbpt, tmph, tmpl
shll Zbpt2, Zbpt, 1
addl Zbp, Zbpt2, Cb

# k' = k + 1
addb kp, k, 1

# test = ((Za' * Za') + (Zb' * Zb') > 4)
# test = ((((Za' * Za') + (Zb' * Zb')) >> 16) > 4)
mulll Zal, Zap, Zap
mulhul Zah, Zap, Zap
mulll Zbl, Zbp, Zbp
mulhul Zbh, Zbp, Zbp
addl sl, Zal, Zbl
addl sh, Zah, Zbh
# 4 == 1 << 18 [Eina_F16p16] == 262144
cmpgtsl rl, sl, 262144
cmpgtsl rh, sh, 0
convlw tmpl, rl
convlw tmph, rh
convwb tl, tmpl
convwb th, tmph
orb test, tl, th

.function color_conversion
.dest 4 px
.n mult 1024
.source 1 k
# N128 = (Eina_F16p16) 128 / N
.param 4 N128 Eina_F16p16
# N255 = (Eina_F16p16) 255 / N
.param 4 N255 Eina_F16p16
.temp 2 kw
.temp 4 kl

.temp 4 redl
.temp 2 redw
.temp 1 red

.temp 4 k255N
.temp 4 greenl
.temp 2 greenw
.temp 1 green

.temp 4 bluel
.temp 2 bluew
.temp 1 blue

.temp 2 rg
.temp 2 ba

# red = 128 - k * 128 / N
convubw kw, k
convuwl kl, kw
mulll redl, N128, kl
# 128 == 128 << 16 == 8388608 [Eina_F16p16]
subl redl, 8388608, redl
convhlw redw, redl
convwb red, redw

# green = 174 - k * 255 / N; if (green < 0) green+=256;
mulll k255N, N255, kl
# 174 == 174 << 16 == 11403264 [Eina_F16p16]
subl greenl, 11403264, k255N
convhlw greenw, greenl
convwb green, greenw

# blue = 128 - k * 255 / N; if (blue < 0) blue+=256;
# 128 == 128 << 16 == 8388608 [Eina_F16p16]
subl bluel, 8388608, k255N
convhlw bluew, bluel
convwb blue, bluew

# px = (red << 24) + (green << 16) + (blue << 8) + 255;
mergebw rg, red, green
mergebw ba, blue, 255
mergewl px, rg, ba
