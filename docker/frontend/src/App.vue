<template>
  <div class="container">
    <h1>PDF转图片工具</h1>
    
    <!-- 上传区域 -->
    <div class="upload-area" 
         @dragover.prevent 
         @drop.prevent="handleDrop"
         :class="{ 'drag-active': isDragging }">
      <input 
        type="file" 
        ref="fileInput" 
        @change="handleFileSelect" 
        accept=".pdf"
        style="display: none"
      />
      <button @click="triggerFileInput" class="upload-btn">
        选择PDF文件
      </button>
      <p>或将文件拖拽到此区域</p>
      <p class="tip">支持 .pdf 格式</p>
    </div>

    <!-- 加载状态 -->
    <div v-if="loading" class="loading">
      <div class="spinner"></div>
      <p>正在处理中，请稍候...</p>
    </div>

    <!-- 错误提示 -->
    <div v-if="error" class="error">
      {{ error }}
      <button @click="error = ''">关闭</button>
    </div>

    <!-- 图片展示区域 -->
    <div v-if="images.length > 0" class="result">
      <h2>转换结果</h2>
      <div class="image-grid">
        <div v-for="(img, index) in images" :key="index" class="image-card">
          <img :src="img.url" :alt="`第${index + 1}页`" 
               @error="handleImageError(img)" />
          <div class="image-info">
            <span>第 {{ index + 1 }} 页</span>
            <button @click="downloadImage(img.name)" class="download-btn">
              下载
            </button>
          </div>
        </div>
      </div>
      
      <!-- 批量下载 -->
      <div class="batch-actions">
        <button @click="downloadAll" class="batch-download-btn">
          批量下载全部 (ZIP)
        </button>
      </div>
    </div>
  </div>
</template>

<script>
import axios from 'axios'
import JSZip from 'jszip'
import { saveAs } from 'file-saver'

// API配置 - 使用完整的后端地址
const API_BASE_URL = 'http://10.64.72.141:8000'  // 改为你后端的实际IP

export default {
  name: 'PdfToImage',
  data() {
    return {
      isDragging: false,
      loading: false,
      error: '',
      images: [],
      fileId: null
    }
  },
  methods: {
    triggerFileInput() {
      this.$refs.fileInput.click()
    },
    
    handleFileSelect(event) {
      const file = event.target.files[0]
      if (file) {
        this.uploadFile(file)
      }
    },
    
    handleDrop(event) {
      this.isDragging = false
      const file = event.dataTransfer.files[0]
      if (file && file.type === 'application/pdf') {
        this.uploadFile(file)
      } else {
        this.error = '请拖拽PDF文件'
      }
    },
    
    async uploadFile(file) {
      this.loading = true
      this.error = ''
      this.images = []
      
      const formData = new FormData()
      formData.append('file', file)
      
      try {
        console.log('开始上传文件:', file.name)
        
        const response = await axios.post(`${API_BASE_URL}/upload/`, formData, {
          headers: {
            'Content-Type': 'multipart/form-data'
          },
          timeout: 120000  // 增加超时时间到120秒
        })
        
        console.log('上传响应:', response.data)
        
        // 检查响应数据
        if (response.data && response.data.images) {
          this.fileId = response.data.file_id
          
          // 构建图片URL
          this.images = response.data.images.map(imagePath => {
            // 如果路径已经是完整URL，直接使用；否则拼接
            const imageUrl = imagePath.startsWith('http') 
              ? imagePath 
              : `${API_BASE_URL}${imagePath}`
            
            return {
              name: imagePath.split('/').pop(),
              url: imageUrl
            }
          })
          
          console.log('图片列表:', this.images)
        } else {
          throw new Error('响应数据格式错误')
        }
        
      } catch (err) {
        console.error('上传失败详情:', err)
        if (err.response) {
          // 服务器返回了错误响应
          console.error('错误响应:', err.response.data)
          this.error = err.response.data?.detail || `服务器错误: ${err.response.status}`
        } else if (err.request) {
          // 请求已发送但没有收到响应
          console.error('无响应:', err.request)
          this.error = '无法连接到服务器，请检查后端服务是否运行'
        } else {
          // 其他错误
          this.error = err.message || '上传失败，请重试'
        }
      } finally {
        this.loading = false
      }
    },
    
    handleImageError(img) {
      console.error('图片加载失败:', img.url)
      // 可以显示一个占位图
      img.url = 'data:image/svg+xml;base64,PHN2ZyB3aWR0aD0iMjAwIiBoZWlnaHQ9IjIwMCIgeG1sbnM9Imh0dHA6Ly93d3cudzMub3JnLzIwMDAvc3ZnIj48cmVjdCB3aWR0aD0iMjAwIiBoZWlnaHQ9IjIwMCIgZmlsbD0iI2VlZSIvPjx0ZXh0IHg9IjUwJSIgeT0iNTAlIiBmb250LWZhbWlseT0iQXJpYWwiIGZvbnQtc2l6ZT0iMTYiIGZpbGw9IiM5OTkiIHRleHQtYW5jaG9yPSJtaWRkbGUiIGR5PSIuM2VtIj7lm77niYfliqDovb3kv6Hmga88L3RleHQ+PC9zdmc+'
    },
    
    async downloadImage(fileName) {
      try {
        const response = await axios({
          url: `${API_BASE_URL}/download/${fileName}`,
          method: 'GET',
          responseType: 'blob'
        })
        
        const blob = new Blob([response.data], { type: 'image/png' })
        const link = document.createElement('a')
        const url = URL.createObjectURL(blob)
        link.href = url
        link.download = fileName
        document.body.appendChild(link)
        link.click()
        document.body.removeChild(link)
        URL.revokeObjectURL(url)
        
      } catch (err) {
        console.error('下载失败:', err)
        this.error = '下载失败，请重试'
      }
    },
    
    async downloadAll() {
      if (this.images.length === 0) return
      
      this.loading = true
      const zip = new JSZip()
      
      try {
        // 并行下载所有图片
        const downloadPromises = this.images.map(async (img) => {
          const response = await axios({
            url: img.url,
            method: 'GET',
            responseType: 'blob'
          })
          return { name: img.name, data: response.data }
        })
        
        const downloadedFiles = await Promise.all(downloadPromises)
        
        // 添加到ZIP
        downloadedFiles.forEach(file => {
          zip.file(file.name, file.data)
        })
        
        // 生成ZIP文件
        const content = await zip.generateAsync({ type: 'blob' })
        saveAs(content, `pdf_images_${this.fileId || Date.now()}.zip`)
        
      } catch (err) {
        console.error('批量下载失败:', err)
        this.error = '批量下载失败，请重试'
      } finally {
        this.loading = false
      }
    }
  }
}
</script>

<style scoped>
/* 保持之前的样式不变 */
.container {
  max-width: 1200px;
  margin: 0 auto;
  padding: 20px;
  font-family: 'Arial', sans-serif;
}

h1 {
  text-align: center;
  color: #333;
  margin-bottom: 30px;
}

.upload-area {
  border: 2px dashed #ccc;
  border-radius: 8px;
  padding: 40px;
  text-align: center;
  background: #f9f9f9;
  transition: all 0.3s ease;
  cursor: pointer;
}

.upload-area.drag-active {
  border-color: #409eff;
  background: #ecf5ff;
}

.upload-btn {
  background: #409eff;
  color: white;
  border: none;
  padding: 12px 24px;
  border-radius: 4px;
  font-size: 16px;
  cursor: pointer;
  transition: background 0.3s;
}

.upload-btn:hover {
  background: #66b1ff;
}

.tip {
  font-size: 12px;
  color: #999;
  margin-top: 10px;
}

.loading {
  text-align: center;
  margin-top: 30px;
}

.spinner {
  border: 3px solid #f3f3f3;
  border-top: 3px solid #409eff;
  border-radius: 50%;
  width: 40px;
  height: 40px;
  animation: spin 1s linear infinite;
  margin: 0 auto 10px;
}

@keyframes spin {
  0% { transform: rotate(0deg); }
  100% { transform: rotate(360deg); }
}

.error {
  background: #fef0f0;
  color: #f56c6c;
  padding: 12px;
  border-radius: 4px;
  margin-top: 20px;
  display: flex;
  justify-content: space-between;
  align-items: center;
}

.error button {
  background: none;
  border: none;
  color: #f56c6c;
  cursor: pointer;
  font-size: 16px;
}

.result {
  margin-top: 40px;
}

.result h2 {
  color: #333;
  margin-bottom: 20px;
}

.image-grid {
  display: grid;
  grid-template-columns: repeat(auto-fill, minmax(250px, 1fr));
  gap: 20px;
}

.image-card {
  border: 1px solid #e4e7ed;
  border-radius: 8px;
  overflow: hidden;
  background: white;
  transition: transform 0.2s, box-shadow 0.2s;
}

.image-card:hover {
  transform: translateY(-2px);
  box-shadow: 0 2px 12px 0 rgba(0, 0, 0, 0.1);
}

.image-card img {
  width: 100%;
  height: 200px;
  object-fit: cover;
  display: block;
}

.image-info {
  padding: 12px;
  display: flex;
  justify-content: space-between;
  align-items: center;
  background: #fafafa;
}

.download-btn {
  background: #67c23a;
  color: white;
  border: none;
  padding: 6px 12px;
  border-radius: 4px;
  cursor: pointer;
  font-size: 12px;
  transition: background 0.3s;
}

.download-btn:hover {
  background: #85ce61;
}

.batch-actions {
  margin-top: 30px;
  text-align: center;
}

.batch-download-btn {
  background: #e6a23c;
  color: white;
  border: none;
  padding: 12px 24px;
  border-radius: 4px;
  cursor: pointer;
  font-size: 16px;
  transition: background 0.3s;
}

.batch-download-btn:hover {
  background: #ebb563;
}
</style>