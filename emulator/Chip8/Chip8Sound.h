#ifndef CHIP8QTEMULATOR_CHIP8SOUND_H
#define CHIP8QTEMULATOR_CHIP8SOUND_H

#include <QAudioFormat>
#include <QIODevice>
#include <QtMath>
#include <atomic>

class Chip8Sound : public QIODevice {
    public:
        Chip8Sound( const qreal frequency, const QAudioFormat &format, QObject *parent = nullptr )
            : QIODevice(parent), m_format(format),m_phaseStep(2.0 * M_PI * frequency / format.sampleRate()),
            m_gainStep(1.0 / (0.005 * format.sampleRate())), m_framesPerTick(format.sampleRate() / 60)
        {
            Q_ASSERT(m_format.sampleFormat() == QAudioFormat::Int16);
            QIODevice::open(ReadOnly);
        }

        void beepFor( const int ticks ) {
            const qint64 wanted = static_cast<qint64>(ticks) * m_framesPerTick;
            qint64 current = m_remainingFrames.load(std::memory_order_relaxed);
            while (
                wanted > current
                && !m_remainingFrames.compare_exchange_weak(current, wanted, std::memory_order_relaxed)
            ) { }
        }

        void stop() { m_remainingFrames.store(0, std::memory_order_relaxed); }

        [[nodiscard]] bool isSequential() const override { return true; }
        [[nodiscard]] qint64 bytesAvailable() const override { return 4096 + QIODevice::bytesAvailable(); }

    protected:
        qint64 readData( char *data, const qint64 maxlen ) override {
            const int channels = m_format.channelCount();
            const int bytesPerFrame = m_format.bytesPerFrame();
            const qint64 frames = maxlen / bytesPerFrame;
            const qint64 remaining = qMax<qint64>(0, m_remainingFrames.load(std::memory_order_relaxed));
            auto *out = reinterpret_cast<qint16 *>(data);

            for ( qint64 i = 0; i < frames; ++i )
            {
                if ( const qreal target = i < remaining ? 1.0 : 0.0; m_gain < target )
                    m_gain = qMin(target, m_gain + m_gainStep);
                else if (m_gain > target) m_gain = qMax(target, m_gain - m_gainStep);

                const auto sample = static_cast<qint16>(kAmplitude * m_gain * qSin(m_phase));
                for ( int c = 0; c < channels; ++c )
                    *out++ = sample;

                m_phase += m_phaseStep;
                if (m_phase >= 2.0 * M_PI) m_phase -= 2.0 * M_PI;
            }

            if ( const qint64 consumed = qMin(frames, remaining); consumed > 0 )
                m_remainingFrames.fetch_sub(consumed, std::memory_order_relaxed);

            return frames * bytesPerFrame;
        }

        qint64 writeData(const char *, qint64) override { return 0; }

    private:
        static constexpr qreal kAmplitude = 0.25 * 32767.0;

        QAudioFormat m_format;
        qreal m_phaseStep;
        qreal m_gainStep;
        qint64 m_framesPerTick;
        qreal m_phase = 0.0;
        qreal m_gain = 0.0;
        std::atomic<qint64> m_remainingFrames{0};
};

#endif // CHIP8QTEMULATOR_CHIP8SOUND_H