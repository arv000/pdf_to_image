<template>
  <div class="pdf-uploader">
    <div class="upload-area" 
         @dragover.prevent 
         @drop.prevent="handleDrop"
         @click="triggerFileInput">
      <input type="file" 
             ref="fileInput" 
             @change="handleFileChange" 
             accept=".pdf" 
             style="display: none">
      
      <div v-if="!file">
        <i class="upload-icon">📄</i>
        <p>点击或拖拽 PDF 文件到此区域</p>
        <p class="hint">支持 .pdf 格式</p>
      </div>
      
      <div v-else class="file-info">
        <i class="file-icon">📄</i>
        <p>{{ file.name }}</p>
        <p class="file-size">{{ formatFileSize(file.size) }}</p>
        <button @click.stop="removeFile" class="remove-btn">移除</button>
      </div>
    </div>
    
    <div v-if="uploadProgress > 0" class="progress-bar">
      <div class="progress" :style="{ width: uploadProgress + '%' }"></div>
      <span>{{ uploadProgress }}%</span>
    </div>
    
    <button @click="uploadFile" 
            :disabled="!file || uploading"
            class="upload-btn">
      {{ uploading ? '上传中...' : '上传 PDF' }}
    </button>
    
    <div v-if="uploadResult" class="result">
      <p v-if="uploadResult.success" class="success">
        ✅ 上传成功！文件ID: {{ uploadResult.fileId }}
      </p>
      <p v-else class="error">
        ❌ 上传失败：{{ uploadResult.message }}
      </p>
    </div>
  </div>
</template>

<script>
export default {
  name: 'PdfUploader',
  data() {
    return {
      file: null,
      uploading: false,
      uploadProgress: 0,
      uploadResult: null
    }
  },
  methods: {
    triggerFileInput() {
      this.$refs.fileInput.click()
    },
    
    handleFileChange(event) {
      const selectedFile = event.target.files[0]
      this.validateAndSetFile(selectedFile)
    },
    
    handleDrop(event) {
      const droppedFile = event.dataTransfer.files[0]
      this.validateAndSetFile(droppedFile)
    },
    
    validateAndSetFile(file) {
      if (!file) return
      
      // 验证文件类型
      if (file.type !== 'application/pdf') {
        this.showError('请选择 PDF 文件')
        return
      }
      
      // 验证文件大小（例如限制 10MB）
      const maxSize = 10 * 1024 * 1024
      if (file.size > maxSize) {
        this.showError('文件大小不能超过 10MB')
        return
      }
      
      this.file = file
      this.uploadResult = null
    },
    
    removeFile() {
      this.file = null
      this.$refs.fileInput.value = ''
      this.uploadProgress = 0
    },
    
    async uploadFile() {
      if (!this.file) return
      
      this.uploading = true
      this.uploadProgress = 0
      
      const formData = new FormData()
      formData.append('file', this.file)
      
      try {
        const response = await this.$axios.post('/api/upload/pdf', formData, {
          headers: {
            'Content-Type': 'multipart/form-data'
          },
          onUploadProgress: (progressEvent) => {
            this.uploadProgress = Math.round(
              (progressEvent.loaded * 100) / progressEvent.total
            )
          }
        })
        
        this.uploadResult = {
          success: true,
          fileId: response.data.fileId,
          message: '上传成功'
        }
        
        // 触发父组件事件
        this.$emit('upload-success', response.data)
        
      } catch (error) {
        console.error('上传失败:', error)
        this.uploadResult = {
          success: false,
          message: error.response?.data?.message || '网络错误'
        }
      } finally {
        this.uploading = false
      }
    },
    
    formatFileSize(bytes) {
      if (bytes === 0) return '0 Bytes'
      const k = 1024
      const sizes = ['Bytes', 'KB', 'MB', 'GB']
      const i = Math.floor(Math.log(bytes) / Math.log(k))
      return parseFloat((bytes / Math.pow(k, i)).toFixed(2)) + ' ' + sizes[i]
    },
    
    showError(message) {
      // 可以使用 Element UI 或自定义提示
      alert(message)
    }
  }
}
</script>

<style scoped>
.pdf-uploader {
  max-width: 500px;
  margin: 0 auto;
  padding: 20px;
}

.upload-area {
  border: 2px dashed #ccc;
  border-radius: 8px;
  padding: 40px;
  text-align: center;
  cursor: pointer;
  transition: all 0.3s;
  background-color: #fafafa;
}

.upload-area:hover {
  border-color: #409eff;
  background-color: #f0f9ff;
}

.upload-icon, .file-icon {
  font-size: 48px;
  display: inline-block;
  margin-bottom: 10px;
}

.hint {
  font-size: 12px;
  color: #999;
  margin-top: 10px;
}

.file-info {
  position: relative;
}

.remove-btn {
  background-color: #f56c6c;
  color: white;
  border: none;
  padding: 5px 10px;
  border-radius: 4px;
  cursor: pointer;
  margin-top: 10px;
}

.remove-btn:hover {
  background-color: #f78989;
}

.progress-bar {
  margin-top: 20px;
  background-color: #f0f0f0;
  border-radius: 4px;
  height: 30px;
  position: relative;
  overflow: hidden;
}

.progress {
  background-color: #409eff;
  height: 100%;
  transition: width 0.3s;
  display: flex;
  align-items: center;
  justify-content: center;
  color: white;
}

.upload-btn {
  margin-top: 20px;
  width: 100%;
  padding: 10px;
  background-color: #409eff;
  color: white;
  border: none;
  border-radius: 4px;
  cursor: pointer;
  font-size: 16px;
}

.upload-btn:disabled {
  background-color: #a0cfff;
  cursor: not-allowed;
}

.result {
  margin-top: 20px;
  padding: 10px;
  border-radius: 4px;
}

.success {
  color: #67c23a;
  background-color: #f0f9eb;
  padding: 10px;
}

.error {
  color: #f56c6c;
  background-color: #fef0f0;
  padding: 10px;
}
</style>