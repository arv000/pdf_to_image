from fastapi import FastAPI, UploadFile, File, HTTPException
from fastapi.responses import FileResponse
from fastapi.middleware.cors import CORSMiddleware
import fitz  # PyMuPDF
import os
import uuid
import logging

# 配置日志
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

app = FastAPI()

# 配置CORS - 解决跨域问题
app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],  # 开发环境允许所有来源
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

UPLOAD_DIR = "/data/uploads"
OUTPUT_DIR = "/data/output"

os.makedirs(UPLOAD_DIR, exist_ok=True)
os.makedirs(OUTPUT_DIR, exist_ok=True)

@app.post("/upload/")
async def upload_pdf(file: UploadFile = File(...)):
    # 验证文件类型
    if not file.filename.endswith('.pdf'):
        raise HTTPException(status_code=400, detail="只支持PDF文件")
    
    file_id = str(uuid.uuid4())
    pdf_path = os.path.join(UPLOAD_DIR, f"{file_id}.pdf")
    
    try:
        # 保存上传的PDF文件
        content = await file.read()
        with open(pdf_path, "wb") as f:
            f.write(content)
        
        logger.info(f"PDF saved: {pdf_path}, size: {len(content)} bytes")
        
        # 使用 PyMuPDF 转换
        pdf_document = fitz.open(pdf_path)
        output_files = []
        
        for page_num in range(len(pdf_document)):
            page = pdf_document[page_num]
            # 设置图片质量（分辨率）
            zoom = 2  # 2倍分辨率，提高清晰度
            mat = fitz.Matrix(zoom, zoom)
            pix = page.get_pixmap(matrix=mat)
            
            img_path = os.path.join(OUTPUT_DIR, f"{file_id}_{page_num}.png")
            pix.save(img_path)
            output_files.append(img_path)
            logger.info(f"Image saved: {img_path}")
        
        pdf_document.close()
        
        # 清理临时PDF文件
        os.remove(pdf_path)
        
        # 返回图片的下载URL列表
        image_urls = [f"/download/{os.path.basename(path)}" for path in output_files]
        
        return {
            "file_id": file_id, 
            "images": image_urls
        }
        
    except Exception as e:
        logger.error(f"Error processing PDF: {str(e)}")
        # 清理临时文件
        if os.path.exists(pdf_path):
            os.remove(pdf_path)
        raise HTTPException(status_code=500, detail=f"处理失败: {str(e)}")

@app.get("/download/{file_name}")
def download(file_name: str):
    # 安全检查，防止路径遍历攻击
    if ".." in file_name or "/" in file_name or "\\" in file_name:
        raise HTTPException(status_code=400, detail="非法文件名")
    
    path = os.path.join(OUTPUT_DIR, file_name)
    if not os.path.exists(path):
        raise HTTPException(status_code=404, detail="文件不存在")
    
    return FileResponse(path, media_type="image/png", filename=file_name)

@app.get("/health")
def health_check():
    return {"status": "ok"}

if __name__ == "__main__":
    import uvicorn
    uvicorn.run(app, host="0.0.0.0", port=8000)