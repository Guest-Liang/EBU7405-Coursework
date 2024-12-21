import os
import subprocess

def convert_images_to_headers():
    # 当前文件夹路径
    current_dir = os.getcwd()
    
    # 支持的图片扩展名
    supported_extensions = ['.png', '.jpg', '.jpeg']
    
    # 遍历当前文件夹中的所有文件
    for filename in os.listdir(current_dir):
        # 获取文件的扩展名并检查是否是支持的图片格式
        _, ext = os.path.splitext(filename)
        if ext.lower() in supported_extensions:
            # 输出头文件的文件名
            header_filename = filename.replace(ext, '.h')
            
            try:
                # 使用 xxd -i 命令生成头文件
                with open(header_filename, 'w') as header_file:
                    subprocess.run(['xxd', '-i', filename], stdout=header_file)
                
                print(f"Converted {filename} to {header_filename}")
            except FileNotFoundError:
                print("Error: xxd command not found. Please make sure xxd is installed and available in your PATH.")
                return
            except Exception as e:
                print(f"Failed to convert {filename}: {e}")

if __name__ == "__main__":
    convert_images_to_headers()
