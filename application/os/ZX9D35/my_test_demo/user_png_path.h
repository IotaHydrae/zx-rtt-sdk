#ifndef __UASER_PNG_PATH_H__
#define __UASER_PNG_PATH_H__


#ifdef LPKG_USING_RAMDISK
#define LVGL_DIR "L:/ram/"
#else
#define LVGL_DIR "L:/rodata/"
#endif

#define CONN(x, y) x#y
#define LVGL_PATH(y) CONN(LVGL_DIR, y)

#define	WIFI_ON					LVGL_PATH(ramfs/Wifion.png)
#define PAGE_TIME_BACKGROUND	LVGL_PATH(ramfs/page_time_background.png)

#define	TIMER_IMG2				LVGL_PATH(ramfs/Timer_img2.png)
#define	TIMER_IMG1				LVGL_PATH(ramfs/Timer_img1.png)

#define	PHONE_IMG2				LVGL_PATH(ramfs/Phone_img2.png)
#define	PHONE_IMG1				LVGL_PATH(ramfs/Phone_img1.png)


#define	SET_UP_IMG2				LVGL_PATH(ramfs/Set_up2.png)
#define	SET_UP_IMG1				LVGL_PATH(ramfs/Set_up1.png)	

#define	CONTROL_IMG1			LVGL_PATH(ramfs/Control1.png)			
#define	CONTROL_IMG2			LVGL_PATH(ramfs/Control2.png)			
#define	CONTROL_IMG3			LVGL_PATH(ramfs/Control3.png)


#define	DRAWER_IMG1				LVGL_PATH(ramfs/Drawer1.png)
#define	DRAWER_IMG2				LVGL_PATH(ramfs/Drawer2.png)
#define	DRAWER_IMG3				LVGL_PATH(ramfs/Drawer3.png)
#define	DRAWER_IMG4				LVGL_PATH(ramfs/Drawer4.png)
#define	DRAWER_IMG5				LVGL_PATH(ramfs/Drawer5.png)

#define	SCENE_IMG1				LVGL_PATH(ramfs/Scene1.png)
#define	SCENE_IMG2				LVGL_PATH(ramfs/Scene2.png)
#define	SCENE_IMG3				LVGL_PATH(ramfs/Scene3.png)
#define	SCENE_IMG4				LVGL_PATH(ramfs/Scene4.png)
#define	SCENE_IMG5				LVGL_PATH(ramfs/Scene5.png)


#define	LEFT_IMG1				LVGL_PATH(ramfs/left_img1.png)
#define	LEFT_IMG2				LVGL_PATH(ramfs/left_img2.png)
#define	LEFT_IMG6				LVGL_PATH(ramfs/left_img6.png)
#define	LEFT_IMG4				LVGL_PATH(ramfs/left_img4.png)

#define	IMG_BOTTOM1				LVGL_PATH(ramfs/img_bottom1.png)
#define	IMG_BOTTOM2				LVGL_PATH(ramfs/img_bottom2.png)


#define MID_SRC_1_IMG1                  LVGL_PATH(ramfs/mid_src_1_img1.png)
#define MID_SRC_1_IMG2                  LVGL_PATH(ramfs/mid_src_1_img2.png)
#define MID_SRC_1_BACKGROUD_LEFT        LVGL_PATH(ramfs/mid_src_1_Background_left.png)
#define MID_SRC_1_BACKGROUD_RIGHT       LVGL_PATH(ramfs/mid_src_1_Background_right.png)
#define MID_SRC_1_IMG5                  LVGL_PATH(ramfs/mid_src_1_img5.png)
// #define MID_SRC_1_IMG6                  LVGL_PATH(ramfs/mid_src_1_img6.png)
#define MID_SRC_1_IMG9                  LVGL_PATH(ramfs/mid_src_1_img9.png)
#define MID_SRC_1_IMG10                 LVGL_PATH(ramfs/mid_src_1_img10.png)

// #define MID_SRC_2_IMG1                  LVGL_PATH(ramfs/mid_src_2_img1.png)
#define MID_SRC_2_IMG2                  LVGL_PATH(ramfs/mid_src_2_img2.png)
// #define MID_SRC_2_IMG3                  LVGL_PATH(ramfs/mid_src_2_img3.png)
#define MID_SRC_2_IMG4                  LVGL_PATH(ramfs/mid_src_2_img4.png)
#define MID_SRC_2_IMG5                  LVGL_PATH(ramfs/mid_src_2_img5.png)
// #define MID_SRC_2_IMG6                  LVGL_PATH(ramfs/mid_src_2_img6.png)
#define MID_SRC_2_IMG7                  LVGL_PATH(ramfs/mid_src_2_img7.png)
#define MID_SRC_2_IMG8                  LVGL_PATH(ramfs/mid_src_2_img8.png)
#define MID_SRC_2_IMG9                  LVGL_PATH(ramfs/mid_src_2_img9.png)
// #define MID_SRC_2_IMG10                  LVGL_PATH(ramfs/mid_src_2_img10.png)


// #define MID_SRC_3_IMG_1                 LVGL_PATH(ramfs/mid_src_3_img1.png)
#define MID_SRC_3_IMG_2                 LVGL_PATH(ramfs/mid_src_3_img2.png)
#define MID_SRC_3_IMG_3                 LVGL_PATH(ramfs/mid_src_3_img3.png)
#define MID_SRC_3_IMG_4                 LVGL_PATH(ramfs/mid_src_3_img4.png)
#define MID_SRC_3_IMG_5                 LVGL_PATH(ramfs/mid_src_3_img5.png)
// #define MID_SRC_3_IMG_6                 LVGL_PATH(ramfs/mid_src_3_img6.png)
// #define MID_SRC_3_IMG_7                 LVGL_PATH(ramfs/mid_src_3_img7.png)
// #define MID_SRC_3_IMG_8                 LVGL_PATH(ramfs/mid_src_3_img8.png)
// #define MID_SRC_3_IMG_9                 LVGL_PATH(ramfs/mid_src_3_img9.png)
// #define MID_SRC_3_IMG_10                 LVGL_PATH(ramfs/mid_src_3_img10.png)

#endif
