
opencl_sources = [
  'alien-map.cl',
  'bilateral-filter-fast.cl',
  'bilateral-filter.cl',
  'box-blur.cl',
  'box-max.cl',
  'box-min.cl',
  'boxblur-1d.cl',
  'brightness-contrast.cl',
  'c2g.cl',
  'checkerboard.cl',
  'color-exchange.cl',
  'color-temperature.cl',
  'color-to-alpha.cl',
  'colors-8bit-lut.cl',
  'colors.cl',
  'contrast-curve.cl',
  'diffraction-patterns.cl',
  'edge-laplace.cl',
  'edge-sobel.cl',
  'gaussian-blur-selective.cl',
  'gblur-1d.cl',
  'hue-chroma.cl',
  'invert-linear.cl',
  'levels.cl',
  'mono-mixer.cl',
  'motion-blur-circular.cl',
  'motion-blur-linear.cl',
  'noise-cell.cl',
  'noise-hsv.cl',
  'noise-hurl.cl',
  'noise-reduction.cl',
  'noise-simplex.cl',
  'oilify.cl',
  'opacity.cl',
  'pixelize.cl',
  'posterize.cl',
  'random.cl',
  'red-eye-removal.cl',
  'shadows-highlights-correction.cl',
  'snn-mean.cl',
  'stretch-contrast.cl',
  'svg-src-over.cl',
  'texturize-canvas.cl',
  'threshold.cl',
  'value-invert.cl',
  'video-degradation.cl',
  'vignette.cl',
  'weighted-blend.cl',
]

cltostring = files('cltostring.py')

opencl_headers = files()
foreach source : opencl_sources
  opencl_headers += custom_target(source + '.h',
    input : source,
    output: source +'.h',
    command: [ python, cltostring, '@INPUT@', '@OUTPUT@' ],
  )
endforeach
