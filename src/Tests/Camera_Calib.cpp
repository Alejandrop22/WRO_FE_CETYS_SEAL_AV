#include "esp_camera.h"
#include <WiFi.h>
#include "esp_http_server.h"

// =================================================================
// 1. CONFIGURACIÓN RED WIFI
// =================================================================
const char* ssid = "iPhone";
const char* password = "Alejandro123";

// =================================================================
// 2. DEFINICIÓN DE PINES (ESP32-S3 OV3660)
// =================================================================
#define PWDN_GPIO_NUM     -1
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM     15
#define SIOD_GPIO_NUM      4
#define SIOC_GPIO_NUM      5

#define Y9_GPIO_NUM       16
#define Y8_GPIO_NUM       17
#define Y7_GPIO_NUM       18
#define Y6_GPIO_NUM       12
#define Y5_GPIO_NUM       10
#define Y4_GPIO_NUM        8
#define Y3_GPIO_NUM        9
#define Y2_GPIO_NUM       11
#define VSYNC_GPIO_NUM     6
#define HREF_GPIO_NUM      7
#define PCLK_GPIO_NUM     13

// =================================================================
// 3. INTERFAZ WEB COMPLETA
// =================================================================
static const char PROGMEM INDEX_HTML[] = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>ESP32-S3 Detector HSV con Corte de Pista</title>
  <style>
    body { font-family: sans-serif; background: #181825; color: #cdd6f4; text-align: center; margin: 0; padding: 20px; }
    .container { max-width: 800px; margin: 0 auto; background: #1e1e2e; padding: 20px; border-radius: 12px; }
    canvas { width: 100%; height: auto; border-radius: 8px; border: 2px solid #89b4fa; background: #000; }
    .panel { background: #313244; padding: 15px; border-radius: 10px; margin: 15px 0; text-align: left; }
    .control-row { display: flex; align-items: center; justify-content: space-between; margin: 6px 0; }
    .control-row label { width: 45%; font-weight: bold; }
    .control-row input[type=range] { width: 40%; }
    .control-row span { width: 15%; text-align: right; }
    button { background: #89b4fa; color: #1e1e2e; border: none; padding: 12px 20px; font-weight: bold; border-radius: 8px; cursor: pointer; width: 100%; margin: 10px 0; }
    .grid-2 { display: grid; grid-template-columns: 1fr 1fr; gap: 15px; }
    @media (max-width: 600px) { .grid-2 { grid-template-columns: 1fr; } }
  </style>
</head>
<body>
  <div class="container">
    <h2>Detector HSV - Filtro de Pista (ROI Y)</h2>
    <canvas id="canvasDisplay" width="640" height="480"></canvas>

    <!-- CORTADOR DE PISTA -->
    <div class="panel" style="border: 2px solid #74c7ec;">
      <h3 style="color:#74c7ec; margin-top:0;">✂️ Límite de Pista (Ignorar lo de abajo)</h3>
      <div class="control-row">
        <label>Límite Y (Pista %):</label>
        <input type="range" id="roi_y" min="10" max="100" value="45" oninput="updateSliders()">
        <span id="roi_y_val">45%</span>
      </div>
      <small style="color:#a6adc8;">Ajusta el slider hasta que la línea cian punteada quede al final de la pista blanca.</small>
    </div>

    <button id="btnCapturar" onclick="capturar()">📸 Capturar Imagen</button>

    <!-- CONTROLES HSV -->
    <div class="grid-2">
      <!-- VERDE -->
      <div class="panel">
        <h3 style="color:#a6e3a1; margin-top:0;">🟢 Ventana Verde</h3>
        <div class="control-row"><label>H Mín:</label><input type="range" id="gh_min" min="0" max="360" value="55" oninput="updateSliders()"><span id="gh_min_val">55°</span></div>
        <div class="control-row"><label>H Máx:</label><input type="range" id="gh_max" min="0" max="360" value="140" oninput="updateSliders()"><span id="gh_max_val">140°</span></div>
        <div class="control-row"><label>S Mín:</label><input type="range" id="gs_min" min="0" max="100" value="40" oninput="updateSliders()"><span id="gs_min_val">40%</span></div>
        <div class="control-row"><label>S Máx:</label><input type="range" id="gs_max" min="0" max="100" value="100" oninput="updateSliders()"><span id="gs_max_val">100%</span></div>
        <div class="control-row"><label>V Mín:</label><input type="range" id="gv_min" min="0" max="100" value="30" oninput="updateSliders()"><span id="gv_min_val">30%</span></div>
        <div class="control-row"><label>V Máx:</label><input type="range" id="gv_max" min="0" max="100" value="100" oninput="updateSliders()"><span id="gv_max_val">100%</span></div>
      </div>

      <!-- ROJO -->
      <div class="panel">
        <h3 style="color:#f38ba8; margin-top:0;">🔴 Ventana Rojo</h3>
        <div class="control-row"><label>H Mín:</label><input type="range" id="rh_min" min="0" max="360" value="340" oninput="updateSliders()"><span id="rh_min_val">340°</span></div>
        <div class="control-row"><label>H Máx:</label><input type="range" id="rh_max" min="0" max="360" value="1" oninput="updateSliders()"><span id="rh_max_val">1°</span></div>
        <div class="control-row"><label>S Mín:</label><input type="range" id="rs_min" min="0" max="100" value="30" oninput="updateSliders()"><span id="rs_min_val">30%</span></div>
        <div class="control-row"><label>S Máx:</label><input type="range" id="rs_max" min="0" max="100" value="100" oninput="updateSliders()"><span id="rs_max_val">100%</span></div>
        <div class="control-row"><label>V Mín:</label><input type="range" id="rv_min" min="0" max="100" value="25" oninput="updateSliders()"><span id="rv_min_val">25%</span></div>
        <div class="control-row"><label>V Máx:</label><input type="range" id="rv_max" min="0" max="100" value="78" oninput="updateSliders()"><span id="rv_max_val">78%</span></div>
      </div>
    </div>
  </div>

  <script>
    let rawImg = new Image();
    let imgLoaded = false;
    const canvas = document.getElementById('canvasDisplay');
    const ctx = canvas.getContext('2d');

    function capturar() {
      const btn = document.getElementById('btnCapturar');
      btn.disabled = true;
      btn.innerText = 'Capturando...';

      rawImg.crossOrigin = "Anonymous";
      rawImg.src = '/capture?t=' + Date.now();
      rawImg.onload = () => {
        imgLoaded = true;
        btn.disabled = false;
        btn.innerText = '📸 Capturar Imagen';
        processBlobsAndRender();
      };
    }

    function rgbToHsv(r, g, b) {
      r /= 255; g /= 255; b /= 255;
      let max = Math.max(r, g, b), min = Math.min(r, g, b);
      let h, s, v = max;
      let d = max - min;
      s = max === 0 ? 0 : d / max;
      if (max === min) { h = 0; } 
      else {
        switch (max) {
          case r: h = (g - b) / d + (g < b ? 6 : 0); break;
          case g: h = (b - r) / d + 2; break;
          case b: h = (r - g) / d + 4; break;
        }
        h /= 6;
      }
      return { h: Math.round(h * 360), s: Math.round(s * 100), v: Math.round(v * 100) };
    }

    function inHsvRange(h, s, v, minH, maxH, minS, maxS, minV, maxV) {
      if (s < minS || s > maxS || v < minV || v > maxV) return false;
      return minH <= maxH ? (h >= minH && h <= maxH) : (h >= minH || h <= maxH);
    }

    function processBlobsAndRender() {
      if (!imgLoaded) return;

      canvas.width = rawImg.naturalWidth || rawImg.width;
      canvas.height = rawImg.naturalHeight || rawImg.height;
      ctx.drawImage(rawImg, 0, 0);

      const w = canvas.width;
      const h = canvas.height;
      const imgData = ctx.getImageData(0, 0, w, h);
      const pixels = imgData.data;

      // ✂️ LÍMITE DE PISTA (THRESHOLD Y)
      const roiPercent = parseInt(document.getElementById('roi_y').value);
      const maxY = Math.floor(h * (roiPercent / 100));

      const g_hmin = parseInt(document.getElementById('gh_min').value);
      const g_hmax = parseInt(document.getElementById('gh_max').value);
      const g_smin = parseInt(document.getElementById('gs_min').value);
      const g_smax = parseInt(document.getElementById('gs_max').value);
      const g_vmin = parseInt(document.getElementById('gv_min').value);
      const g_vmax = parseInt(document.getElementById('gv_max').value);

      const r_hmin = parseInt(document.getElementById('rh_min').value);
      const r_hmax = parseInt(document.getElementById('rh_max').value);
      const r_smin = parseInt(document.getElementById('rs_min').value);
      const r_smax = parseInt(document.getElementById('rs_max').value);
      const r_vmin = parseInt(document.getElementById('rv_min').value);
      const r_vmax = parseInt(document.getElementById('rv_max').value);

      let g_count = 0, g_minX = w, g_maxX = 0, g_minY = h, g_maxY = 0, g_sumX = 0, g_sumY = 0;
      let r_count = 0, r_minX = w, r_maxX = 0, r_minY = h, r_maxY = 0, r_sumX = 0, r_sumY = 0;

      const overlay = ctx.createImageData(w, h);
      const oPix = overlay.data;

      // RECORREMOS DESDE maxY HACIA ABAJO (IGNORA LO DE ARRIBA DEL LÍMITE)
      for (let y = maxY; y < h; y += 2) {
        for (let x = 0; x < w; x += 2) {
          const idx = (y * w + x) * 4;
          const hsv = rgbToHsv(pixels[idx], pixels[idx + 1], pixels[idx + 2]);

          if (inHsvRange(hsv.h, hsv.s, hsv.v, g_hmin, g_hmax, g_smin, g_smax, g_vmin, g_vmax)) {
            g_count++;
            if (x < g_minX) g_minX = x; if (x > g_maxX) g_maxX = x;
            if (y < g_minY) g_minY = y; if (y > g_maxY) g_maxY = y;
            g_sumX += x; g_sumY += y;
            oPix[idx] = 0; oPix[idx+1] = 255; oPix[idx+2] = 0; oPix[idx+3] = 120;
          } else if (inHsvRange(hsv.h, hsv.s, hsv.v, r_hmin, r_hmax, r_smin, r_smax, r_vmin, r_vmax)) {
            r_count++;
            if (x < r_minX) r_minX = x; if (x > r_maxX) r_maxX = x;
            if (y < r_minY) r_minY = y; if (y > r_maxY) r_maxY = y;
            r_sumX += x; r_sumY += y;
            oPix[idx] = 255; oPix[idx+1] = 0; oPix[idx+2] = 0; oPix[idx+3] = 120;
          }
        }
      }

      const tempC = document.createElement('canvas');
      tempC.width = w; tempC.height = h;
      tempC.getContext('2d').putImageData(overlay, 0, 0);
      ctx.drawImage(tempC, 0, 0);

      // Dibujar línea de corte cian
      ctx.strokeStyle = '#74c7ec';
      ctx.lineWidth = 3;
      ctx.setLineDash([8, 6]);
      ctx.beginPath();
      ctx.moveTo(0, maxY);
      ctx.lineTo(w, maxY);
      ctx.stroke();
      ctx.setLineDash([]);

      ctx.fillStyle = '#74c7ec';
      ctx.font = 'bold 14px Arial';
      ctx.fillText('✂️ Límite de Pista', 10, Math.max(18, maxY - 6));

      // Bounding Boxes
      ctx.lineWidth = 3;
      ctx.font = 'bold 16px Arial';

      if (g_count > 15) {
        ctx.strokeStyle = '#00ff66'; ctx.fillStyle = '#00ff66';
        ctx.strokeRect(g_minX, g_minY, g_maxX - g_minX, g_maxY - g_minY);
        ctx.beginPath(); ctx.arc(Math.round(g_sumX / g_count), Math.round(g_sumY / g_count), 6, 0, 2 * Math.PI); ctx.fill();
        ctx.fillText(`Blob Verde (${g_count} px)`, g_minX, Math.max(20, g_minY - 8));
      }

      if (r_count > 15) {
        ctx.strokeStyle = '#ff3366'; ctx.fillStyle = '#ff3366';
        ctx.strokeRect(r_minX, r_minY, r_maxX - r_minX, r_maxY - r_minY);
        ctx.beginPath(); ctx.arc(Math.round(r_sumX / r_count), Math.round(r_sumY / r_count), 6, 0, 2 * Math.PI); ctx.fill();
        ctx.fillText(`Blob Rojo (${r_count} px)`, r_minX, Math.max(20, r_minY - 8));
      }
    }

    function updateSliders() {
      ['gh_min','gh_max','gs_min','gs_max','gv_min','gv_max','rh_min','rh_max','rs_min','rs_max','rv_min','rv_max'].forEach(id => {
        document.getElementById(id + '_val').innerText = document.getElementById(id).value + (id.includes('h') ? '°' : '%');
      });
      document.getElementById('roi_y_val').innerText = document.getElementById('roi_y').value + '%';
      processBlobsAndRender();
    }
  </script>
</body>
</html>
)rawliteral";

// =================================================================
// 4. SERVIDOR HTTP Y CAMARA
// =================================================================
httpd_handle_t camera_httpd = NULL;

esp_err_t index_handler(httpd_req_t *req) {
  httpd_resp_set_type(req, "text/html");
  return httpd_resp_send(req, INDEX_HTML, strlen(INDEX_HTML));
}

esp_err_t capture_handler(httpd_req_t *req) {
  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) { return ESP_FAIL; }
  httpd_resp_set_type(req, "image/jpeg");
  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
  esp_err_t res = httpd_resp_send(req, (const char *)fb->buf, fb->len);
  esp_camera_fb_return(fb);
  return res;
}

void setup() {
  Serial.begin(115200);

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM; config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM; config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM; config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM; config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM; config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM; config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM; config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM; config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size = FRAMESIZE_SVGA;
  config.jpeg_quality = 12;
  config.fb_count = 2;
  config.grab_mode = CAMERA_GRAB_LATEST;

  esp_camera_init(&config);

  sensor_t * s = esp_camera_sensor_get();
  if (s) { s->set_vflip(s, 1); }

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { delay(500); }

  httpd_config_t server_config = HTTPD_DEFAULT_CONFIG();
  httpd_uri_t index_uri = { "/", HTTP_GET, index_handler, NULL };
  httpd_uri_t capture_uri = { "/capture", HTTP_GET, capture_handler, NULL };

  if (httpd_start(&camera_httpd, &server_config) == ESP_OK) {
    httpd_register_uri_handler(camera_httpd, &index_uri);
    httpd_register_uri_handler(camera_httpd, &capture_uri);
  }

  Serial.print("IP: http://"); Serial.println(WiFi.localIP());
}

void loop() { delay(10000); }
