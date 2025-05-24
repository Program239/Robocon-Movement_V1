import cv2

# Open default camera (your laptop's webcam)
cap = cv2.VideoCapture(0)

while True:
    ret, frame = cap.read()  # Capture frame-by-frame
    
    if not ret:
        print("Failed to grab frame")
        break
    
    cv2.imshow("Camera Test", frame)  # Display the frame

    if cv2.waitKey(1) & 0xFF == ord('q'):  # Press 'q' to exit
        break

cap.release()
cv2.destroyAllWindows()
