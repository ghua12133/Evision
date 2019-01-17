#include "EvisionView.h"
#include "QDebug"
#include "QMessageBox"
#include "EvisionUtils.h"
#include "Calibrater.h"
#include "Matcher.h"
#include <qevent.h>
#include <qmimedata.h>
#include <QFileDialog>
#include "RFinterface.h"
#include "StereoCamera.h"
#include "Camera.h"
// 浮点数判等
// ulp: units in the last place.
template <typename T>
typename std::enable_if<!std::numeric_limits<T>::is_integer, bool>::type
IsAlmostEqual(T x, T y, int ulp = 2)
{
	// the machine epsilon has to be scaled to the magnitude of the values used
	// and multiplied by the desired precision in ULPs (units in the last place)
	return std::abs(x - y) < std::numeric_limits<T>::epsilon() * std::abs(x + y) * ulp
		// unless the result is subnormal
		|| std::abs(x - y) < std::numeric_limits<T>::min();
}

//构造函数
EvisionView::EvisionView(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	setAcceptDrops(true);
	msgLabel = new QLabel;
	msgLabel->setMinimumSize(msgLabel->sizeHint());
	msgLabel->setAlignment(Qt::AlignHCenter);
	msgLabel->setText(QStringLiteral("就绪"));
	statusBar()->addWidget(msgLabel);
	statusBar()->setStyleSheet(QString("QStatusBar::item{border: 0px}"));

	m_entity = EvisionParamEntity::getInstance();
	m_controller = new EvisionController();


	connect(m_entity, SIGNAL(paramChanged_StatusBar()), this, SLOT(onParamChanged_StatusBarText()), Qt::QueuedConnection);

}


//显示单目相机视图
void EvisionView::onCamera()
{
	Camera * _camera = new Camera();
	ui.mdiArea->addSubWindow(_camera);
	_camera->show();
}
//显示双目相机视图
void EvisionView::onStereoCamera()
{
	StereoCamera * _stereoCamera = new StereoCamera();
	ui.mdiArea->addSubWindow(_stereoCamera);
	_stereoCamera->show();
}

//显示点云
void EvisionView::onShowPointCloud()
{
	//TODO:need refactor
	QFileDialog * fileDialog2 = new QFileDialog();
	fileDialog2->setWindowTitle(QStringLiteral("请选择点云文件"));
	fileDialog2->setNameFilter(QStringLiteral("点云文件(*.xml *.yml *.yaml)"));
	fileDialog2->setFileMode(QFileDialog::ExistingFile);
	if (fileDialog2->exec() == QDialog::Accepted)
	{
		QString xyzFile = fileDialog2->selectedFiles().at(0);
		cv::Mat xyz = StereoMatch::readXYZ(xyzFile.toStdString().c_str());
	}
	else
	{
		QMessageBox::information(NULL, QStringLiteral("错误"), QStringLiteral("请选择有效的点云文件!"));
		return;
	}
	//点云获取ok,准备显示
}
//显示标定视图
void EvisionView::on_action_calibrate_view()
{
	Calibrater * m_calibrate = new Calibrater();
	ui.mdiArea->addSubWindow(m_calibrate);
	m_calibrate->show();
}
//显示立体匹配视图
void EvisionView::on_action_stereoMatch_view()
{
	Matcher * m_matcher = new Matcher();
	ui.mdiArea->addSubWindow(m_matcher);
	m_matcher->show();
}
//显示交互式测距视图
void EvisionView::on_action_Measure_view()
{
	RFinterface * _Rfinterface = new RFinterface();
	ui.mdiArea->addSubWindow(_Rfinterface);
	_Rfinterface->show();
}

//调试方法
void EvisionView::onTestAlltheParam()
{

}
//状态栏更新
void EvisionView::onParamChanged_StatusBarText()
{
	msgLabel->setText(m_entity->getStatusBarText());
}
//文件被拖到窗口区域上
void EvisionView::dragEnterEvent(QDragEnterEvent * event)
{
	if (event->mimeData()->hasFormat("text/uri-list"))
	{
		event->acceptProposedAction();
		//QMessageBox::information(NULL, QStringLiteral("消息"), QStringLiteral("文件被拖上来"));
		m_entity->setStatusBarText("Drop the file for open!");
	}
}
//文件在窗口区域上被放下
void EvisionView::dropEvent(QDropEvent * event)
{
	m_entity->setStatusBarText(QStringLiteral("就绪"));
	//QMessageBox::information(NULL, QStringLiteral("消息"), QStringLiteral("文件被释放在窗口上"));
	QList<QUrl> urls = event->mimeData()->urls();
	if (urls.isEmpty())
	{
		return;
	}
	else if(urls.size()>1)
	{
		QMessageBox::information(NULL, QStringLiteral("消息"), QStringLiteral("多于一个文件"));
	}else if(urls.size()==1)
	{
		QMessageBox::information(NULL, QStringLiteral("消息"), QStringLiteral("一个文件"));
		//文件分类识别和打开
		QFileInfo fileinfo(urls[0].toString());
		if (!fileinfo.isFile())//不是文件
		{
			return;
		}
		else
		{
			if (fileinfo.suffix() == "png"|| fileinfo.suffix() == "jpg"||
				fileinfo.suffix() == "jpeg")
			{
				
			}
		}
	}
		
}
//鼠标释放事件
void EvisionView::mouseReleaseEvent(QMouseEvent * event)
{
	if (event->button() == Qt::LeftButton)
	{
		m_entity->setStatusBarText(QStringLiteral("就绪"));
	}
}

