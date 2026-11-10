const { useRef, useEffect } = React;

function ColorButton({ color, onChange }) {
    const buttonRef = useRef(null);
	const pickerRef = useRef(null);
	const killRecursion = useRef(false);
	
	
    useEffect(() => {
        const picker = new ColorPicker(buttonRef.current, {
            toggleStyle: 'button',
            color: color,
            submitMode: 'instant',
            enableAlpha: false
        });
		
		pickerRef.current = picker;

        picker.on('pick', selected => {
            onChange(selected.string('hex'));
	     });

        return () => {
            picker.destroy();
			pickerRef.current = null;
        };
    }, []);

    useEffect(() => {
		if(pickerRef.current && pickerRef.current.color.string('hex').toLowerCase() !== color.toLowerCase()) {
			pickerRef.current.setColor(color);
		}
    }, [color]);

    return (
        <button
            ref={buttonRef}
            type="button"
            aria-label="Choose colour"
            style={{ backgroundColor: color }}
        />
    );
}
