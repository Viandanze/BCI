# 数字孪生每日学习简报

## Day 11 | 2026-05-10（周日）

---

## 📌 今日学习主题

**MQTT 传感器数据采集 + Three.js 实时可视化系统搭建**

从传感器到 Web 可视化的完整数据流实操

---

## 🔥 大厂应用速览

### 1. MQTT 在数字孪生中的核心地位

| 维度 | MQTT 特性 | 数字孪生价值 |
|------|-----------|-------------|
| **资源消耗** | 几十 KB 客户端库 | 可运行在微控制器/传感器节点 |
| **网络容错** | QoS 等级 + 持久会话 | 网络中断时消息不丢失 |
| **架构解耦** | 发布/订阅模式 | 设备无需知道订阅者 |
| **带宽占用** | 极低（单传感器~100KB/天） | 减轻工厂无线网络压力 |

**典型案例**：美的荆州洗衣机工厂，117 台 KUKA 机器人通过 MQTT 接入 KUKA Connect 系统，实现设备状态实时监控。

### 2. 库卡 2025 工博会五大智能体（MQTT 数据全量采集）

```
KUKA CONNECT IOT
├── 连接超 6000 台机器人
├── MQTT 协议实现数据全量采集
├── B/S 架构云端管理
└── 设备运维智能体
```

### 3. 数字孪生技术成熟度（2026）

- **Gartner 2025 Hype Cycle**：数字孪生进入**生产力成熟期**
- **市场规模**：2026 年突破 350-500 亿美元
- **核心趋势**：从"可视化"→"可执行+可决策"

---

## 📚 核心技术栈

```
[物理世界]                    [数字世界]
  ↓                            ↑
┌─────────┐    MQTT     ┌──────────────┐
│ DHT11   │ ────────→   │ Mosquitto    │
│ HC-SR04 │             │ Broker       │
│ BMP280  │             └──────┬───────┘
└─────────┘                    ↓
                          ┌──────────────┐
                          │ Python       │
                          │ Subscriber   │
                          └──────┬───────┘
                                 ↓
                          ┌──────────────┐
                          │ SQLite/      │
                          │ InfluxDB     │
                          └──────┬───────┘
                                 ↓
                          ┌──────────────┐
                          │ Three.js     │
                          │ 实时渲染     │
                          └──────────────┘
```

---

## 🎯 今日实战任务（硬软结合）

### 任务概述

**搭建 MQTT 传感器数据采集 + Three.js 可视化系统**

实现：从 DHT11 温湿度传感器采集数据 → MQTT 发布 → Python 订阅处理 → Three.js 3D 可视化

### 任务分解

#### Part 1：MQTT 环境搭建（30 分钟）

```bash
# 1. 安装 Mosquitto
sudo apt update && sudo apt install -y mosquitto mosquitto-clients

# 2. 启动服务
sudo systemctl start mosquitto
sudo systemctl enable mosquitto

# 3. 测试本地发布/订阅
# 终端1：订阅主题
mosquitto_sub -t "test/sensor"

# 终端2：发布消息
mosquitto_pub -t "test/sensor" -m '{"temp": 25.5, "humidity": 60}'
```

#### Part 2：Python 数据采集与发布（45 分钟）

**文件：`sensor_publisher.py`**

```python
import paho.mqtt.client as mqtt
import time
import json
import Adafruit_DHT as dht

# 传感器配置
SENSOR = dht.DHT11
PIN = 4  # GPIO 4

# MQTT 配置
broker = "localhost"
port = 1883
topic = "digital_twin/sensors/dht11"

def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("✅ MQTT 连接成功")
    else:
        print(f"❌ 连接失败，RC={rc}")

def read_sensor():
    humidity, temp = dht.read_retry(SENSOR, PIN)
    return {
        "temperature": temp,
        "humidity": humidity,
        "timestamp": int(time.time())
    }

def main():
    client = mqtt.Client()
    client.on_connect = on_connect
    client.connect(broker, port, 60)
    client.loop_start()
    
    while True:
        data = read_sensor()
        if data["temperature"] is not None:
            payload = json.dumps(data)
            client.publish(topic, payload)
            print(f"📤 已发布: {payload}")
        time.sleep(5)  # 5秒采集一次

if __name__ == "__main__":
    main()
```

**安装依赖**：
```bash
pip3 install paho-mqtt Adafruit_DHT
```

#### Part 3：Python 订阅 + 数据处理（30 分钟）

**文件：`data_subscriber.py`**

```python
import paho.mqtt.client as mqtt
import sqlite3
import json

broker = "localhost"
port = 1883
topic = "digital_twin/sensors/dht11"

# SQLite 初始化
conn = sqlite3.connect('sensor_data.db')
conn.execute('''
    CREATE TABLE IF NOT EXISTS readings (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        temperature REAL,
        humidity REAL,
        timestamp INTEGER
    )
''')
conn.close()

def on_message(client, userdata, msg):
    try:
        data = json.loads(msg.payload)
        conn = sqlite3.connect('sensor_data.db')
        conn.execute('''
            INSERT INTO readings (temperature, humidity, timestamp)
            VALUES (?, ?, ?)
        ''', (data['temperature'], data['humidity'], data['timestamp']))
        conn.commit()
        conn.close()
        print(f"💾 已存储: T={data['temperature']}°C, H={data['humidity']}%")
    except Exception as e:
        print(f"❌ 存储失败: {e}")

def on_connect(client, userdata, flags, rc):
    client.subscribe(topic)
    print(f"📥 已订阅: {topic}")

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
client.connect(broker, port, 60)
client.loop_forever()
```

#### Part 4：Three.js 实时 3D 可视化（60 分钟）

**文件：`visualization.html`**

```html
<!DOCTYPE html>
<html>
<head>
    <title>数字孪生 - 温湿度实时监控</title>
    <style>
        body { margin: 0; background: #1a1a2e; color: #fff; font-family: Arial; }
        #container { width: 100vw; height: 100vh; }
        #info { position: absolute; top: 20px; left: 20px; z-index: 100; }
        .sensor-data { font-size: 24px; margin: 10px 0; }
        .temp { color: #ff6b6b; }
        .humidity { color: #4ecdc4; }
    </style>
</head>
<body>
    <div id="info">
        <h2>🌡️ 传感器实时数据</h2>
        <div class="sensor-data temp">温度: <span id="temp">--</span>°C</div>
        <div class="sensor-data humidity">湿度: <span id="humidity">--</span>%</div>
    </div>
    <div id="container"></div>
    
    <script src="https://cdnjs.cloudflare.com/ajax/libs/three.js/r128/three.min.js"></script>
    <script src="https://cdn.jsdelivr.net/npm/mqtt/dist/mqtt.min.js"></script>
    <script>
        // Three.js 场景
        const scene = new THREE.Scene();
        scene.background = new THREE.Color(0x1a1a2e);
        
        const camera = new THREE.PerspectiveCamera(75, window.innerWidth/window.innerHeight, 0.1, 1000);
        camera.position.z = 5;
        
        const renderer = new THREE.WebGLRenderer();
        renderer.setSize(window.innerWidth, window.innerHeight);
        document.getElementById('container').appendChild(renderer.domElement);
        
        // 创建温度指示器（球体大小随温度变化）
        const tempGeometry = new THREE.SphereGeometry(1, 32, 32);
        const tempMaterial = new THREE.MeshPhongMaterial({ 
            color: 0xff6b6b,
            emissive: 0x330000
        });
        const tempSphere = new THREE.Mesh(tempGeometry, tempMaterial);
        scene.add(tempSphere);
        
        // 创建湿度指示器（环形）
        const humidityGeometry = new THREE.TorusGeometry(1.5, 0.1, 16, 100);
        const humidityMaterial = new THREE.MeshPhongMaterial({ color: 0x4ecdc4 });
        const humidityTorus = new THREE.Mesh(humidityGeometry, humidityMaterial);
        humidityTorus.rotation.x = Math.PI / 2;
        humidityTorus.position.y = -2;
        scene.add(humidityTorus);
        
        // 光源
        const light = new THREE.PointLight(0xffffff, 1, 100);
        light.position.set(5, 5, 5);
        scene.add(light);
        
        const ambientLight = new THREE.AmbientLight(0x404040);
        scene.add(ambientLight);
        
        // MQTT.js 连接（浏览器端）
        const client = mqtt.connect('ws://localhost:9001'); // WebSocket 端口
        
        client.on('connect', () => {
            console.log('📡 MQTT WebSocket 已连接');
            client.subscribe('digital_twin/sensors/dht11');
        });
        
        client.on('message', (topic, message) => {
            const data = JSON.parse(message.toString());
            document.getElementById('temp').textContent = data.temperature;
            document.getElementById('humidity').textContent = data.humidity;
            
            // 更新 3D 场景
            // 温度影响球体大小 (0-50°C -> 0.5-2倍)
            const tempScale = 0.5 + (data.temperature / 50) * 1.5;
            tempSphere.scale.set(tempScale, tempScale, tempScale);
            
            // 湿度影响环形旋转速度
            humidityTorus.rotation.z += (data.humidity / 100) * 0.1;
            
            // 颜色渐变
            const hue = (1 - data.temperature / 50) * 0.3;
            tempMaterial.color.setHSL(hue, 0.8, 0.5);
        });
        
        // 动画循环
        function animate() {
            requestAnimationFrame(animate);
            tempSphere.rotation.y += 0.01;
            humidityTorus.rotation.z += 0.02;
            renderer.render(scene, camera);
        }
        animate();
        
        // 窗口调整
        window.addEventListener('resize', () => {
            camera.aspect = window.innerWidth / window.innerHeight;
            camera.updateProjectionMatrix();
            renderer.setSize(window.innerWidth, window.innerHeight);
        });
    </script>
</body>
</html>
```

**配置 MQTT WebSocket（用于浏览器）**：

编辑 `/etc/mosquitto/mosquitto.conf`：
```
listener 9001
protocol websockets
allow_anonymous true
```

重启服务：
```bash
sudo systemctl restart mosquitto
```

---

## 🔍 验收机制

### ✅ 验收标准

| 阶段 | 验收点 | 自测方法 |
|------|--------|----------|
| MQTT | Broker 运行正常 | `mosquitto_sub` 能收到消息 |
| 采集 | DHT11 数据读取正常 | 控制台每秒输出温湿度 |
| 存储 | SQLite 数据写入 | 查询数据库有记录 |
| 可视化 | Three.js 渲染正常 | 浏览器无报错，球体随数据动 |
| 联调 | 全链路数据流 | 传感器变化→3D场景响应 |

### 🎯 交付物检查清单

- [ ] `sensor_publisher.py` - 传感器采集与 MQTT 发布
- [ ] `data_subscriber.py` - MQTT 订阅与数据存储
- [ ] `visualization.html` - Three.js 3D 可视化
- [ ] `sensor_data.db` - SQLite 数据库（含历史数据）
- [ ] 截图/录屏 - 3D 场景响应传感器数据

### 📊 费曼复述检查（用自己的话回答）

1. MQTT 的发布/订阅模式如何实现设备解耦？
2. 为什么数字孪生系统选择 MQTT 而不是 HTTP？
3. Three.js 中如何根据传感器数据动态更新 3D 对象？

---

## 📚 扩展阅读

| 资源 | 链接 | 备注 |
|------|------|------|
| ThingsBoard 官方教程 | https://thingsboard.io/docs/ | MQTT + 仪表盘完整方案 |
| Mosquitto 文档 | https://mosquitto.org/man/mosquitto-conf-5.html | WebSocket 配置 |
| Three.js 入门 | https://threejs.org/docs/ | 官方文档 |

---

## ⏭️ 明日预告

**Day 12 - 数据滤波与可视化优化**

- 卡尔曼滤波算法实现
- 传感器数据去噪
- Three.js 图表绑定（温度曲线）

---

*本简报由数字孪生日课系统自动生成*
*BruceZeng - 2026-05-10*
