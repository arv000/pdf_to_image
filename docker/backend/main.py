from fastapi import FastAPI, UploadFile, File
from fastapi.responses import FileResponse
from pdf2image import convert_from_path
import os
import uuid

app = FastAPI()

UPLOAD_DIR = "/data/uploads"
OUTPUT_DIR = "/data/output"

os.makedirs(UPLOAD_DIR, exist_ok=True)
os.makedirs(OUTPUT_DIR, exist_ok=True)

@app.post("/upload/")
async def upload_pdf(file: UploadFile = File(...)):
    file_id = str(uuid.uuid4())
    pdf_path = os.path.join(UPLOAD_DIR, f"{file_id}.pdf")

    with open(pdf_path, "wb") as f:
        f.write(await file.read())

    images = convert_from_path(pdf_path)

    output_files = []
    for i, img in enumerate(images):
        img_path = os.path.join(OUTPUT_DIR, f"{file_id}_{i}.png")
        img.save(img_path, "PNG")
        output_files.append(img_path)

    return {"file_id": file_id, "images": output_files}


@app.get("/download/{file_name}")
def download(file_name: str):
    path = os.path.join(OUTPUT_DIR, file_name)
    return FileResponse(path)