import cv2
from ultralytics import YOLO

# Load YOLOv8 pretrained model
model = YOLO('yolov8n.pt')  # 'n' is the nano version: fast and lightweight

# Start webcam video stream
cap = cv2.VideoCapture(0)

while True:
    success, frame = cap.read()
    if not success:
        break
    
    # YOLO detection on frame
    results = model(frame, stream=True)

    # Show detected objects on frame
    for result in results:
        boxes = result.boxes.xyxy.cpu().numpy()
        confidences = result.boxes.conf.cpu().numpy()
        class_ids = result.boxes.cls.cpu().numpy()
        
        for box, confidence, class_id in zip(boxes, confidences, class_ids):
            x1, y1, x2, y2 = box.astype(int)
            label = model.names[int(class_id)]
            
            # Draw bounding box and label
            cv2.rectangle(frame, (x1, y1), (x2, y2), (255, 0, 0), 2)
            cv2.putText(frame, f"{label} {confidence:.2f}", 
                        (x1, y1-10), cv2.FONT_HERSHEY_SIMPLEX, 
                        0.5, (255, 0, 0), 2)

    # Display output frame
    cv2.imshow("YOLO Real-time Detection", frame)

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()
