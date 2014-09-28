#ifndef NETWORKUPDATER_H
#define NETWORKUPDATER_H

#include <QObject>
#include <QThread>
class DeviceEdit;
class NetworkUpdater : public QObject
{
		Q_OBJECT
	public:
		explicit NetworkUpdater(DeviceEdit* parent = 0);
		~NetworkUpdater();

	signals:
		void deviceChanged(QString);
		void deviceNameChanged(QString,QString);
		void deviceProtocolChanged(QString);
		void newDeviceAdded(QString);
		void namespaceLoaded(QString);
		
		void disableTree();
		void enableTree();
		
	public slots:
		void update();

	private:
		QThread thrd;
		DeviceEdit* ed;
};

#endif // NETWORKUPDATER_H
